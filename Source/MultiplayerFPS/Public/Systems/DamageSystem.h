#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DamageSystem.generated.h"

/**
 * Damage system for handling hit feedback and damage effects
 */
UCLASS()
class MULTIPLAYERFPS_API UDamageSystem : public UObject
{
	GENERATED_BODY()

public:
	UDamageSystem();

	// ========== Hit Feedback ==========
	
	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void ApplyHitFeedback(AActor* DamagedActor, const FVector& HitLocation, const FVector& HitNormal, float Damage);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void SpawnBloodEffect(const FVector& Location, const FVector& Normal);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void SpawnImpactDecal(const FVector& Location, const FVector& Normal);

	// ========== Damage Calculation ==========
	
	UFUNCTION(BlueprintCallable, Category = "Damage")
	static float CalculateDamage(float BaseDamage, float Distance, bool bIsHeadshot = false);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	static bool IsHeadshot(const FVector& HitLocation, AActor* HitActor);

	// ========== Screen Effects ==========
	
	UFUNCTION(BlueprintCallable, Category = "Damage")
	static void ApplyScreenDamageEffect(APlayerController* PlayerController, float Damage);

private:
	// Static assets (would be loaded from asset manager in real implementation)
	static UParticleSystem* BloodParticleSystem;
	static UMaterialInterface* ImpactDecalMaterial;
	static USoundBase* HitSound;
};