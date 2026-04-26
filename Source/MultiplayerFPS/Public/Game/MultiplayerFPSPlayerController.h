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

	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetKDRatio() const { return Deaths > 0 ? (float)Kills / Deaths : Kills; }
};
