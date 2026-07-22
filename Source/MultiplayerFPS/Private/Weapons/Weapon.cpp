#include "Weapons/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/DamageSystem.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create skeletal mesh component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Initialize ammo
	CurrentAmmo = MagazineCapacity;
	TotalAmmo = 300;
	LastFireTime = 0.0f;
	bIsFiring = false;
	bIsReloading = false;

	// Enable replication
	bReplicates = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, CurrentAmmo);
	DOREPLIFETIME(AWeapon, TotalAmmo);
	DOREPLIFETIME(AWeapon, bIsReloading);
}

void AWeapon::Fire()
{
	if (!HasAmmo() || bIsReloading)
	{
		if (!HasAmmo())
		{
			UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		}
		return;
	}

	// Check fire rate
	if (GetWorld()->GetTimeSeconds() - LastFireTime < FireRate)
	{
		return;
	}

	// Perform raycast
	PerformRaycast();

	// Play effects
	PlayMuzzleFlash();
	PlayFireSound();

	// Play animation
	if (FireMontage && MeshComponent)
	{
		MeshComponent->PlayAnimation(FireMontage, false);
	}

	// Update ammo
	CurrentAmmo--;
	LastFireTime = GetWorld()->GetTimeSeconds();

	UE_LOG(LogTemp, Warning, TEXT("Weapon fired! Ammo: %d"), CurrentAmmo);
}

void AWeapon::StopFiring()
{
	bIsFiring = false;
}

void AWeapon::PerformRaycast()
{
	if (!MeshComponent)
	{
		return;
	}

	// Get muzzle location (approximate)
	FVector StartLocation = MeshComponent->GetComponentLocation();
	FVector EndLocation = StartLocation + (MeshComponent->GetComponentRotation().Vector() * Range);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	// Perform line trace
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		if (HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitResult.GetActor()->GetName());
			
			// Calculate damage with falloff and headshot
			float Distance = FVector::Dist(StartLocation, HitResult.ImpactPoint);
			bool bIsHeadshot = UDamageSystem::IsHeadshot(HitResult.ImpactPoint, HitResult.GetActor());
			float ActualDamage = UDamageSystem::CalculateDamage(Damage, Distance, bIsHeadshot);
			
			// Apply damage to hit actor
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor != GetOwner())
			{
				HitActor->TakeDamage(ActualDamage, FDamageEvent(), nullptr, this);
				
				// Apply hit feedback
				UDamageSystem::ApplyHitFeedback(HitActor, HitResult.ImpactPoint, HitResult.ImpactNormal, ActualDamage);
			}

			// Spawn impact effect
			SpawnBulletImpact(HitResult.ImpactPoint, HitResult.ImpactNormal);
		}
	}
}

void AWeapon::Reload()
{
	if (CanReload())
	{
		StartReload();
	}
}

bool AWeapon::CanReload() const
{
	return !bIsReloading && CurrentAmmo < MagazineCapacity && TotalAmmo > 0;
}

void AWeapon::StartReload()
{
	bIsReloading = true;
	ReloadStartTime = GetWorld()->GetTimeSeconds();

	// Play reload sound
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}

	// Play reload animation
	if (ReloadMontage && MeshComponent)
	{
		MeshComponent->PlayAnimation(ReloadMontage, false);
	}

	// Schedule finish reload
	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AWeapon::FinishReload,
		ReloadTime,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Starting reload..."));
}

void AWeapon::FinishReload()
{
	if (TotalAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo left to reload!"));
		bIsReloading = false;
		return;
	}

	int32 AmmoToLoad = FMath::Min(MagazineCapacity - CurrentAmmo, TotalAmmo);
	CurrentAmmo += AmmoToLoad;
	TotalAmmo -= AmmoToLoad;

	bIsReloading = false;

	UE_LOG(LogTemp, Warning, TEXT("Weapon reloaded! Current: %d, Total: %d"), CurrentAmmo, TotalAmmo);
}

void AWeapon::AddAmmo(int32 Amount)
{
	TotalAmmo += Amount;
}

bool AWeapon::HasAmmo() const
{
	return CurrentAmmo > 0;
}

void AWeapon::PlayMuzzleFlash()
{
	if (MuzzleFlash && MeshComponent)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, MeshComponent, TEXT("Muzzle"));
	}
}

void AWeapon::PlayFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void AWeapon::SpawnBulletImpact(const FVector& Location, const FVector& Normal)
{
	if (BulletImpactEffect)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		GetWorld()->SpawnActor<AActor>(BulletImpactEffect, Location, Normal.Rotation(), SpawnParams);
	}
}
