#include "Gameplay/LevelProgression.h"

ULevelProgression::ULevelProgression()
{
	LoadHighScore();
}

void ULevelProgression::InitializeLevel(int32 LevelNumber)
{
	CurrentLevel = LevelNumber;
	bLevelComplete = false;
	CurrentScore = 0;
	Objectives.Empty();
	CompletedObjectives.Empty();
}

void ULevelProgression::CompleteLevel()
{
	if (!bLevelComplete)
	{
		bLevelComplete = true;
		OnLevelComplete.Broadcast();

		// Update high score
		if (CurrentScore > HighScore)
		{
			HighScore = CurrentScore;
			SaveHighScore();
		}

		UE_LOG(LogTemp, Log, TEXT("Level %d completed with score %d"), CurrentLevel, CurrentScore);
	}
}

void ULevelProgression::AddObjective(const FString& ObjectiveName)
{
	if (!Objectives.Contains(ObjectiveName))
	{
		Objectives.Add(ObjectiveName);
	}
}

void ULevelProgression::CompleteObjective(const FString& ObjectiveName)
{
	if (Objectives.Contains(ObjectiveName) && !CompletedObjectives.Contains(ObjectiveName))
	{
		CompletedObjectives.Add(ObjectiveName);
		OnObjectiveComplete.Broadcast(ObjectiveName);

		UE_LOG(LogTemp, Log, TEXT("Objective completed: %s"), *ObjectiveName);

		// Check if all objectives are complete
		if (AreAllObjectivesComplete())
		{
			CompleteLevel();
		}
	}
}

bool ULevelProgression::AreAllObjectivesComplete() const
{
	return Objectives.Num() > 0 && CompletedObjectives.Num() == Objectives.Num();
}

TArray<FString> ULevelProgression::GetRemainingObjectives() const
{
	TArray<FString> Remaining;
	for (const FString& Objective : Objectives)
	{
		if (!CompletedObjectives.Contains(Objective))
		{
			Remaining.Add(Objective);
		}
	}
	return Remaining;
}

float ULevelProgression::GetDifficultyMultiplier() const
{
	return BaseDifficultyMultiplier + (CurrentLevel - 1) * DifficultyIncreasePerLevel;
}

int32 ULevelProgression::GetEnemyCountForLevel() const
{
	return BaseEnemyCount + (CurrentLevel - 1) * EnemyCountIncreasePerLevel;
}

float ULevelProgression::GetEnemyHealthMultiplier() const
{
	return GetDifficultyMultiplier();
}

float ULevelProgression::GetEnemyDamageMultiplier() const
{
	return GetDifficultyMultiplier();
}

void ULevelProgression::AddScore(int32 Points)
{
	CurrentScore += Points;
}

void ULevelProgression::SaveHighScore()
{
	// Save to game instance or persistent storage
	// For now, just log it
	UE_LOG(LogTemp, Log, TEXT("New high score: %d"), HighScore);
}

void ULevelProgression::LoadHighScore()
{
	// Load from game instance or persistent storage
	// For now, start with 0
	HighScore = 0;
}