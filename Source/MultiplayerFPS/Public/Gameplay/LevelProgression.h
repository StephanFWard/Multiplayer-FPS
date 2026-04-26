#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelProgression.generated.h"

class AMultiplayerFPSCharacter;

/**
 * Level progression system for game flow and difficulty scaling
 */
UCLASS()
class MULTIPLAYERFPS_API ULevelProgression : public UObject
{
	GENERATED_BODY()

public:
	ULevelProgression();

	// Level management
	void InitializeLevel(int32 LevelNumber);
	void CompleteLevel();
	int32 GetCurrentLevel() const { return CurrentLevel; }
	bool IsLevelComplete() const { return bLevelComplete; }

	// Objectives
	void AddObjective(const FString& ObjectiveName);
	void CompleteObjective(const FString& ObjectiveName);
	bool AreAllObjectivesComplete() const;
	TArray<FString> GetRemainingObjectives() const;

	// Difficulty scaling
	float GetDifficultyMultiplier() const;
	int32 GetEnemyCountForLevel() const;
	float GetEnemyHealthMultiplier() const;
	float GetEnemyDamageMultiplier() const;

	// Scoring
	void AddScore(int32 Points);
	int32 GetCurrentScore() const { return CurrentScore; }
	int32 GetHighScore() const { return HighScore; }

	// Events
	DECLARE_MULTICAST_DELEGATE(FOnLevelComplete);
	FOnLevelComplete OnLevelComplete;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnObjectiveComplete, const FString&)
	FOnObjectiveComplete OnObjectiveComplete;

protected:
	// Level state
	UPROPERTY()
	int32 CurrentLevel = 1;

	UPROPERTY()
	bool bLevelComplete = false;

	// Objectives
	UPROPERTY()
	TArray<FString> Objectives;

	UPROPERTY()
	TSet<FString> CompletedObjectives;

	// Scoring
	UPROPERTY()
	int32 CurrentScore = 0;

	UPROPERTY()
	int32 HighScore = 0;

	// Difficulty parameters
	UPROPERTY(EditAnywhere, Category = "Difficulty")
	float BaseDifficultyMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Difficulty")
	float DifficultyIncreasePerLevel = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Difficulty")
	int32 BaseEnemyCount = 5;

	UPROPERTY(EditAnywhere, Category = "Difficulty")
	int32 EnemyCountIncreasePerLevel = 2;

private:
	// Helper functions
	void SaveHighScore();
	void LoadHighScore();
};