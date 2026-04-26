#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameRules.generated.h"

class AMultiplayerFPSCharacter;
class AMultiplayerFPSPlayerController;

/**
 * Enforces game rules and handles game state validation
 */
UCLASS()
class MULTIPLAYERFPS_API UGameRules : public UObject
{
	GENERATED_BODY()

public:
	UGameRules();

	// Rule validation
	bool CanPlayerJoinGame(AMultiplayerFPSPlayerController* PlayerController) const;
	bool CanPlayerLeaveGame(AMultiplayerFPSPlayerController* PlayerController) const;
	bool CanPlayerRespawn(AMultiplayerFPSCharacter* Player) const;
	bool CanPlayerFireWeapon(AMultiplayerFPSCharacter* Player) const;

	// Game state
	bool IsGameActive() const { return bGameActive; }
	void StartGame();
	void EndGame();
	void PauseGame();
	void ResumeGame();

	// Scoring and objectives
	void OnPlayerKilled(AMultiplayerFPSCharacter* Killer, AMultiplayerFPSCharacter* Victim);
	void OnObjectiveCompleted(AMultiplayerFPSPlayerController* Player, const FString& ObjectiveName);
	int32 GetPlayerScore(AMultiplayerFPSPlayerController* Player) const;

	// Anti-cheat
	bool ValidatePlayerAction(AMultiplayerFPSPlayerController* Player, const FString& Action) const;
	void ReportSuspiciousActivity(AMultiplayerFPSPlayerController* Player, const FString& Activity);

	// Events
	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStarted;

	DECLARE_MULTICAST_DELEGATE(FOnGameEnded)
	FOnGameEnded OnGameEnded;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerKilled, AMultiplayerFPSCharacter*)
	FOnPlayerKilled OnPlayerKilledEvent;

protected:
	// Game state
	UPROPERTY()
	bool bGameActive;

	UPROPERTY()
	bool bGamePaused;

	// Rules settings
	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 MaxPlayers = 8;

	UPROPERTY(EditAnywhere, Category = "Rules")
	float RespawnTime = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 KillScore = 10;

	UPROPERTY(EditAnywhere, Category = "Rules")
	int32 ObjectiveScore = 25;

	// Player scores
	TMap<AMultiplayerFPSPlayerController*, int32> PlayerScores;

	// Anti-cheat data
	TMap<AMultiplayerFPSPlayerController*, float> LastActionTimes;
	TMap<AMultiplayerFPSPlayerController*, int32> SuspiciousActivityCount;

private:
	// Helper functions
	bool IsPlayerValid(AMultiplayerFPSPlayerController* Player) const;
	void AwardScore(AMultiplayerFPSPlayerController* Player, int32 Points);
	void CheckWinConditions();
};