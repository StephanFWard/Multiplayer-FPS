#include "Weapons/Weapon.h"
#include "Components/SkeletalMeshComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create skeletal mesh component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Initialize ammo
	CurrentAmmo = MagazineCapacity;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Fire()
{
	if (!HasAmmo())
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
		return;
	}

	// Check fire rate
	if (GetWorld()->GetTimeSeconds() - LastFireTime < FireRate)
	{
		return;
	}

	// Perform raycast
	PerformRaycast();

	// Update ammo
	CurrentAmmo--;
	LastFireTime = GetWorld()->GetTimeSeconds();

	// TODO: Play fire animation and sound

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
			
			// Apply damage to hit actor
			AActor* HitActor = HitResult.GetActor();
			if (HitActor && HitActor != GetOwner())
			{
				HitActor->TakeDamage(Damage, FDamageEvent(), nullptr, this);
			}
		}
	}
}

void AWeapon::Reload()
{
	if (TotalAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo left to reload!"));
		return;
	}

	int32 AmmoToLoad = FMath::Min(MagazineCapacity - CurrentAmmo, TotalAmmo);
	CurrentAmmo += AmmoToLoad;
	TotalAmmo -= AmmoToLoad;

	// TODO: Play reload animation

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
