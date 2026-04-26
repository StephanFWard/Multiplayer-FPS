#include "Systems/AnimationSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"

// Initialize static map
TMap<USkeletalMeshComponent*, FString> UAnimationSystem::ActiveAnimations;

UAnimationSystem::UAnimationSystem()
{
}

void UAnimationSystem::PlayCharacterAnimation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, bool bLoop)
{
	if (!Mesh || !Montage)
	{
		return;
	}

	// Stop current animation if different
	FString* CurrentAnim = ActiveAnimations.Find(Mesh);
	if (CurrentAnim && *CurrentAnim != Montage->GetName())
	{
		Mesh->Stop();
	}

	// Play new animation
	Mesh->PlayAnimation(Montage, bLoop);

	// Track active animation
	ActiveAnimations.Add(Mesh, Montage->GetName());
}

void UAnimationSystem::StopCharacterAnimation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage)
{
	if (!Mesh || !Montage)
	{
		return;
	}

	if (IsAnimationPlaying(Mesh, Montage))
	{
		Mesh->Stop();
		ActiveAnimations.Remove(Mesh);
	}
}

void UAnimationSystem::BlendCharacterAnimations(USkeletalMeshComponent* Mesh, TArray<UAnimMontage*> Montages, float BlendTime)
{
	if (!Mesh || Montages.Num() == 0)
	{
		return;
	}

	// For simplicity, just play the first montage
	// In a real implementation, this would blend multiple animations
	PlayCharacterAnimation(Mesh, Montages[0], false);
}

void UAnimationSystem::UpdateMovementAnimation(USkeletalMeshComponent* Mesh, float Speed, bool bIsAiming, bool bIsSprinting)
{
	if (!Mesh)
	{
		return;
	}

	// This would set animation variables in a real animation blueprint
	// For now, just log the state
	FString State = FString::Printf(TEXT("Speed: %.2f, Aiming: %d, Sprinting: %d"), Speed, bIsAiming, bIsSprinting);
	UE_LOG(LogTemp, Warning, TEXT("Movement Animation State: %s"), *State);
}

void UAnimationSystem::SetMovementDirection(USkeletalMeshComponent* Mesh, const FVector& Direction)
{
	if (!Mesh)
	{
		return;
	}

	// This would set direction variables for blend space
	UE_LOG(LogTemp, Warning, TEXT("Movement Direction: %s"), *Direction.ToString());
}

void UAnimationSystem::PlayWeaponAnimation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage)
{
	if (!Mesh || !Montage)
	{
		return;
	}

	Mesh->PlayAnimation(Montage, false);
}

bool UAnimationSystem::IsAnimationPlaying(USkeletalMeshComponent* Mesh, UAnimMontage* Montage)
{
	if (!Mesh || !Montage)
	{
		return false;
	}

	FString* CurrentAnim = ActiveAnimations.Find(Mesh);
	return CurrentAnim && *CurrentAnim == Montage->GetName();
}