#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimationSystem.generated.h"

/**
 * Animation system for managing character animations and montages
 */
UCLASS()
class MULTIPLAYERFPS_API UAnimationSystem : public UObject
{
	GENERATED_BODY()

public:
	UAnimationSystem();

	// ========== Animation Management ==========
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void PlayCharacterAnimation(class USkeletalMeshComponent* Mesh, UAnimMontage* Montage, bool bLoop = false);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void StopCharacterAnimation(class USkeletalMeshComponent* Mesh, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void BlendCharacterAnimations(class USkeletalMeshComponent* Mesh, TArray<UAnimMontage*> Montages, float BlendTime = 0.2f);

	// ========== Movement Animation ==========
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void UpdateMovementAnimation(class USkeletalMeshComponent* Mesh, float Speed, bool bIsAiming, bool bIsSprinting);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void SetMovementDirection(class USkeletalMeshComponent* Mesh, const FVector& Direction);

	// ========== Weapon Animation ==========
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	static void PlayWeaponAnimation(class USkeletalMeshComponent* Mesh, UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	static bool IsAnimationPlaying(class USkeletalMeshComponent* Mesh, UAnimMontage* Montage);

private:
	// Animation state tracking
	static TMap<USkeletalMeshComponent*, FString> ActiveAnimations;
};