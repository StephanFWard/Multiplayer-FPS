#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerFPSGameMode.generated.h"

/**
 * Main game mode for Multiplayer FPS
 * Handles game rules, player spawning, and game state management
 */
UCLASS()
class MULTIPLAYERFPS_API AMultiplayerFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerFPSGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Player spawn configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float RespawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 MaxPlayers = 8;

	// Game state
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	int32 CurrentPlayerCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	float RoundTime = 600.0f; // 10 minutes

public:
	// Game flow functions
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void EndGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RespawnPlayer(class APlayerController* PlayerController);

	// Getter functions
	UFUNCTION(BlueprintCallable, Category = "Game")
	int32 GetCurrentPlayerCount() const { return CurrentPlayerCount; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	int32 GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	float GetRoundTime() const { return RoundTime; }
};
