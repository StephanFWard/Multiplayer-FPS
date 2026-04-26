#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelManager.generated.h"

class ALevelGenerator;

/**
 * Manages level loading, generation, and transitions
 */
UCLASS()
class MULTIPLAYERFPS_API ULevelManager : public UObject
{
	GENERATED_BODY()

public:
	ULevelManager();

	// Level operations
	bool LoadLevel(const FString& LevelName);
	bool GenerateProceduralLevel(int32 Seed = 0);
	void UnloadCurrentLevel();

	// Level information
	FString GetCurrentLevelName() const;
	bool IsLevelLoaded() const;
	TArray<FVector> GetSpawnPoints() const;
	TArray<FVector> GetObjectiveLocations() const;

	// Level transitions
	void TransitionToLevel(const FString& LevelName);
	void RestartCurrentLevel();

	// Level settings
	void SetDifficulty(float Difficulty);
	float GetDifficulty() const { return CurrentDifficulty; }

	// Events
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelLoaded, const FString&)
	FOnLevelLoaded OnLevelLoaded;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUnloaded, const FString&)
	FOnLevelUnloaded OnLevelUnloaded;

	DECLARE_MULTICAST_DELEGATE(FOnLevelTransition)
	FOnLevelTransition OnLevelTransition;

protected:
	// Current level state
	UPROPERTY()
	FString CurrentLevelName;

	UPROPERTY()
	bool bLevelLoaded;

	// Level data
	UPROPERTY()
	TArray<FVector> SpawnPoints;

	UPROPERTY()
	TArray<FVector> ObjectiveLocations;

	UPROPERTY()
	float CurrentDifficulty;

	// Level generator
	UPROPERTY()
	ALevelGenerator* LevelGenerator;

private:
	// Helper functions
	void InitializeLevelData();
	void CleanupLevelData();
	bool ValidateLevelData() const;
};