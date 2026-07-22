#include "Systems/DamageSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInterface.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Components/DecalComponent.h"

// Initialize static assets
UParticleSystem* UDamageSystem::BloodParticleSystem = nullptr;
UMaterialInterface* UDamageSystem::ImpactDecalMaterial = nullptr;
USoundBase* UDamageSystem::HitSound = nullptr;

UDamageSystem::UDamageSystem()
{
}

void UDamageSystem::ApplyHitFeedback(AActor* DamagedActor, const FVector& HitLocation, const FVector& HitNormal, float Damage)
{
	if (!DamagedActor)
	{
		return;
	}

	// Spawn blood effect for characters
	if (DamagedActor->ActorHasTag(FName("Character")))
	{
		SpawnBloodEffect(HitLocation, HitNormal);
	}
	else
	{
		// Spawn impact effect for environment
		SpawnImpactDecal(HitLocation, HitNormal);
	}

	// Play hit sound
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(DamagedActor, HitSound, HitLocation);
	}

	// Apply screen effect to local player if damaged
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(DamagedActor, 0))
	{
		ApplyScreenDamageEffect(PC, Damage);
	}
}

void UDamageSystem::SpawnBloodEffect(const FVector& Location, const FVector& Normal)
{
	if (BloodParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GWorld,
			BloodParticleSystem,
			Location,
			Normal.Rotation(),
			FVector(1.0f),
			true
		);
	}
}

void UDamageSystem::SpawnImpactDecal(const FVector& Location, const FVector& Normal)
{
	if (ImpactDecalMaterial)
	{
		// Create decal component
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
			GWorld,
			ImpactDecalMaterial,
			FVector(20.0f, 20.0f, 20.0f),
			Location,
			Normal.Rotation(),
			10.0f
		);

		if (Decal)
		{
			Decal->SetFadeScreenSize(0.01f);
		}
	}
}

float UDamageSystem::CalculateDamage(float BaseDamage, float Distance, bool bIsHeadshot)
{
	float DamageMultiplier = 1.0f;

	// Distance falloff
	if (Distance > 1000.0f)
	{
		DamageMultiplier *= FMath::Max(0.5f, 1.0f - (Distance - 1000.0f) / 5000.0f);
	}

	// Headshot bonus
	if (bIsHeadshot)
	{
		DamageMultiplier *= 2.0f;
	}

	return BaseDamage * DamageMultiplier;
}

bool UDamageSystem::IsHeadshot(const FVector& HitLocation, AActor* HitActor)
{
	if (!HitActor)
	{
		return false;
	}

	// Simple headshot detection based on hit location height
	FVector ActorLocation = HitActor->GetActorLocation();
	float HitHeight = HitLocation.Z - ActorLocation.Z;

	// Assume head is above 150 units from center
	return HitHeight > 150.0f;
}

void UDamageSystem::ApplyScreenDamageEffect(APlayerController* PlayerController, float Damage)
{
	if (!PlayerController)
	{
		return;
	}

	// Add camera shake or screen effect here
	// For now, just log
	UE_LOG(LogTemp, Warning, TEXT("Screen damage effect applied: %f"), Damage);
}