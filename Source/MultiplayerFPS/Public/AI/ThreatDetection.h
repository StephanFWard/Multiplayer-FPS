#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThreatDetection.generated.h"

class AMultiplayerFPSCharacter;

/**
 * Threat detection system for AI enemies
 */
UCLASS()
class MULTIPLAYERFPS_API UThreatDetection : public UObject
{
	GENERATED_BODY()

public:
	UThreatDetection();

	// Threat assessment
	float CalculateThreatLevel(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;
	bool IsThreatVisible(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;
	float GetDetectionRange(AMultiplayerFPSCharacter* Observer) const;
	float GetFieldOfView(AMultiplayerFPSCharacter* Observer) const;

	// Detection states
	bool IsDetected(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;
	bool IsInCombatRange(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;

	// Alert system
	void AlertNearbyAI(AMultiplayerFPSCharacter* AlertedBy, AMultiplayerFPSCharacter* Target, float AlertRadius);

protected:
	// Detection parameters
	UPROPERTY(EditAnywhere, Category = "Detection")
	float BaseDetectionRange = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Detection")
	float BaseFieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Detection")
	float CombatRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Detection")
	float AlertRadius = 1500.0f;

	// Threat multipliers
	UPROPERTY(EditAnywhere, Category = "Threat")
	float DistanceThreatMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Threat")
	float VisibilityThreatMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Threat")
	float MovementThreatMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Threat")
	float ShootingThreatMultiplier = 3.0f;

private:
	// Helper functions
	bool PerformLineOfSightCheck(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;
	bool IsInFieldOfView(AMultiplayerFPSCharacter* Observer, AMultiplayerFPSCharacter* Target) const;
	float CalculateDistanceFactor(float Distance, float MaxRange) const;
};