#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerFPSPlayerController.generated.h"

class AMultiplayerFPSCharacter;

/**
 * Custom player controller for multiplayer FPS
 * Handles input mapping, HUD management, and player-specific logic
 */
UCLASS()
class MULTIPLAYERFPS_API AMultiplayerFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMultiplayerFPSPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	AMultiplayerFPSCharacter* ControlledCharacter;

	// HUD reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class AHUD> HUDClass;

public:
	// Player info
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	int32 Deaths = 0;

	// Setter functions
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerName(const FString& NewPlayerName) { PlayerName = NewPlayerName; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	void OnPlayerKilledEnemy() { Kills++; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	void OnPlayerDied() { Deaths++; }

	// Getter functions
	UFUNCTION(BlueprintCallable, Category = "Player")
	FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetKills() const { return Kills; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	int32 GetDeaths() const { return Deaths; }

	// UI Management
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UMultiplayerFPSHUD* GameHUD;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UChatWidget* ChatWidget;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleChat();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHUD();
};
