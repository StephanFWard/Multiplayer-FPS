#include "Pickups/AmmoPickup.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "Weapons/Weapon.h"

AAmmoPickup::AAmmoPickup()
{
	PickupName = "Ammo";
}

void AAmmoPickup::OnPickup(AMultiplayerFPSCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// Give ammo to player's current weapon
	AWeapon* CurrentWeapon = Player->GetCurrentWeapon();
	if (CurrentWeapon)
	{
		CurrentWeapon->AddAmmo(AmmoAmount);
		UE_LOG(LogTemp, Log, TEXT("Player picked up %d ammo for %s"), AmmoAmount, *WeaponType);
	}

	Super::OnPickup(Player);
}

bool AAmmoPickup::CanPickup(AMultiplayerFPSCharacter* Player) const
{
	if (!Super::CanPickup(Player))
	{
		return false;
	}

	// Check if player needs ammo
	AWeapon* CurrentWeapon = Player->GetCurrentWeapon();
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetCurrentAmmo() < CurrentWeapon->GetMagazineCapacity();
	}

	return false;
}
