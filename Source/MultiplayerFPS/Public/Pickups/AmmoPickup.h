#pragma once

#include "CoreMinimal.h"
#include "Pickups/BasePickup.h"
#include "AmmoPickup.generated.h"

/**
 * Ammo pickup that gives ammo to weapons
 */
UCLASS()
class MULTIPLAYERFPS_API AAmmoPickup : public ABasePickup
{
	GENERATED_BODY()

public:
	AAmmoPickup();

	virtual void OnPickup(class AMultiplayerFPSCharacter* Player) override;
	virtual bool CanPickup(class AMultiplayerFPSCharacter* Player) const override;

protected:
	// Ammo properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	FString WeaponType = "Rifle"; // Could be enum in future
};