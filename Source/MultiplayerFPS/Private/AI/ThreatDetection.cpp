#include "AI/ThreatDetection.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AI/BasicAIController.h"

UThreatDetection::UThreatDetection()
{
}

float UThreatDetection::CalculateThreatLevel(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	if (!Observer || !Target)
	{
		return 0.0f;
	}

	float ThreatLevel = 0.0f;
	float Distance = FVector::Dist(Observer->GetActorLocation(), Target->GetActorLocation());

	// Distance factor
	ThreatLevel += CalculateDistanceFactor(Distance, GetDetectionRange(Observer)) * DistanceThreatMultiplier;

	// Visibility factor
	if (IsThreatVisible(Observer, Target))
	{
		ThreatLevel += VisibilityThreatMultiplier;
	}

	// Movement factor (if target is moving fast)
	if (Target->GetVelocity().Size() > 300.0f)
	{
		ThreatLevel += MovementThreatMultiplier;
	}

	// Shooting factor (if target is firing)
	// This would need to be implemented based on weapon firing state

	return ThreatLevel;
}

bool UThreatDetection::IsThreatVisible(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	return PerformLineOfSightCheck(Observer, Target) && IsInFieldOfView(Observer, Target);
}

float UThreatDetection::GetDetectionRange(AMultiplayerFPSCharacter* Observer) const
{
	// Could be modified based on AI type, alertness level, etc.
	return BaseDetectionRange;
}

float UThreatDetection::GetFieldOfView(AMultiplayerFPSCharacter* Observer) const
{
	// Could be modified based on AI type, alertness level, etc.
	return BaseFieldOfView;
}

bool UThreatDetection::IsDetected(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	if (!Observer || !Target)
	{
		return false;
	}

	float Distance = FVector::Dist(Observer->GetActorLocation(), Target->GetActorLocation());
	float DetectionRange = GetDetectionRange(Observer);

	return Distance <= DetectionRange && IsThreatVisible(Observer, Target);
}

bool UThreatDetection::IsInCombatRange(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	if (!Observer || !Target)
	{
		return false;
	}

	float Distance = FVector::Dist(Observer->GetActorLocation(), Target->GetActorLocation());
	return Distance <= CombatRange;
}

void UThreatDetection::AlertNearbyAI(AMultiplayerFPSCharacter* AlertedBy, AMultiplayerFPSCharacter* Target, float AlertRadius)
{
	if (!AlertedBy || !Target)
	{
		return;
	}

	TArray<AActor*> FoundAI;
	UGameplayStatics::GetAllActorsOfClass(AlertedBy->GetWorld(), ABasicAIController::StaticClass(), FoundAI);

	for (AActor* Actor : FoundAI)
	{
		ABasicAIController* AIController = Cast<ABasicAIController>(Actor);
		if (AIController && AIController->GetPawn() != AlertedBy)
		{
			float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), AlertedBy->GetActorLocation());
			if (Distance <= AlertRadius)
			{
				// Alert this AI - could set target or increase alertness
				UE_LOG(LogTemp, Log, TEXT("AI alerted at distance %f"), Distance);
			}
		}
	}
}

bool UThreatDetection::PerformLineOfSightCheck(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	if (!Observer || !Target)
	{
		return false;
	}

	FVector Start = Observer->GetActorLocation() + FVector(0, 0, 50); // Eye level
	FVector End = Target->GetActorLocation() + FVector(0, 0, 50);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Observer);

	bool bHit = Observer->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	if (bHit)
	{
		return HitResult.GetActor() == Target;
	}

	return false;
}

bool UThreatDetection::IsInFieldOfView(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const
{
	if (!Observer || !Target)
	{
		return false;
	}

	FVector ObserverLocation = Observer->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FVector ObserverForward = Observer->GetActorForwardVector();

	FVector DirectionToTarget = (TargetLocation - ObserverLocation).GetSafeNormal();
	float DotProduct = FVector::DotProduct(ObserverForward, DirectionToTarget);

	float FOVRadians = FMath::DegreesToRadians(GetFieldOfView(Observer) / 2.0f);
	float CosFOV = FMath::Cos(FOVRadians);

	return DotProduct >= CosFOV;
}

float UThreatDetection::CalculateDistanceFactor(float Distance, float MaxRange) const
{
	if (Distance >= MaxRange)
	{
		return 0.0f;
	}

	// Closer = higher threat
	return 1.0f - (Distance / MaxRange);
}