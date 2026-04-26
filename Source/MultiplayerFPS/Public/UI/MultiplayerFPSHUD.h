#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerFPSHUD.generated.h"

/**
 * Main HUD widget for the game
 * Displays health, ammo, crosshair, and other UI elements
 */
UCLASS()
class MULTIPLAYERFPS_API UMultiplayerFPSHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UMultiplayerFPSHUD(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// UI Update functions
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateCrosshair(bool bIsAiming);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowHitMarker();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowDamageIndicator(FVector DamageDirection);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdatePlayerList(TArray<FString> PlayerNames);

protected:
	// UI Components
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	class UImage* CrosshairImage;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PlayerListBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* HitMarkerImage;

	// Damage indicators (4 directions)
	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIndicatorUp;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIndicatorDown;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIndicatorLeft;

	UPROPERTY(meta = (BindWidget))
	class UImage* DamageIndicatorRight;

private:
	// Animation timers
	FTimerHandle HitMarkerTimerHandle;
	FTimerHandle DamageIndicatorTimerHandle;

	void HideHitMarker();
	void HideDamageIndicators();
};