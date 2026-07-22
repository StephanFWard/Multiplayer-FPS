#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
	UFUNCTION(BlueprintCallable, Category = "Level")
	bool LoadLevel(const FString& LevelName);

	UFUNCTION(BlueprintCallable, Category = "Level")
	bool GenerateProceduralLevel(int32 Seed = 0);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void UnloadCurrentLevel();

	// Level information
	UFUNCTION(BlueprintCallable, Category = "Level")
	FString GetCurrentLevelName() const;

	UFUNCTION(BlueprintCallable, Category = "Level")
	bool IsLevelLoaded() const;

	UFUNCTION(BlueprintCallable, Category = "Level")
	TArray<FVector> GetSpawnPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Level")
	TArray<FVector> GetObjectiveLocations() const;

	// Level transitions
	UFUNCTION(BlueprintCallable, Category = "Level")
	void TransitionToLevel(const FString& LevelName);

	UFUNCTION(BlueprintCallable, Category = "Level")
	void RestartCurrentLevel();

	// Level settings
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetDifficulty(float Difficulty);

	UFUNCTION(BlueprintCallable, Category = "Level")
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
