#include "Pickups/HealthPickup.h"
#include "Character/MultiplayerFPSCharacter.h"

AHealthPickup::AHealthPickup()
{
	PickupName = "Health";
}

void AHealthPickup::OnPickup(AMultiplayerFPSCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// Restore health to player
	float CurrentHealth = Player->GetHealth();
	float NewHealth = FMath::Min(CurrentHealth + HealthAmount, MaxHealthRestore);
	Player->Heal(HealthAmount);

	UE_LOG(LogTemp, Log, TEXT("Player picked up %f health, now has %f health"), HealthAmount, NewHealth);

	Super::OnPickup(Player);
}

bool AHealthPickup::CanPickup(AMultiplayerFPSCharacter* Player) const
{
	if (!Super::CanPickup(Player))
	{
		return false;
	}

	// Check if player needs health
	float CurrentHealth = Player->GetHealth();
	return CurrentHealth < MaxHealthRestore;
}
