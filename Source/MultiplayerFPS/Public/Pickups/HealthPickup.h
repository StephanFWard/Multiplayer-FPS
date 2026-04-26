#pragma once

#include "CoreMinimal.h"
#include "Pickups/BasePickup.h"
#include "HealthPickup.generated.h"

/**
 * Health pickup that restores player health
 */
UCLASS()
class MULTIPLAYERFPS_API AHealthPickup : public ABasePickup
{
	GENERATED_BODY()

public:
	AHealthPickup();

	virtual void OnPickup(class AMultiplayerFPSCharacter* Player) override;
	virtual bool CanPickup(class AMultiplayerFPSCharacter* Player) const override;

protected:
	// Health properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthAmount = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealthRestore = 100.0f; // Maximum health this pickup can restore to
};