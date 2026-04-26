#include "Level/LevelManager.h"
#include "Level/LevelGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ULevelManager::ULevelManager()
{
	bLevelLoaded = false;
	CurrentDifficulty = 1.0f;
	LevelGenerator = nullptr;
}

bool ULevelManager::LoadLevel(const FString& LevelName)
{
	if (LevelName.IsEmpty())
	{
		return false;
	}

	// Unload current level first
	if (bLevelLoaded)
	{
		UnloadCurrentLevel();
	}

	// Load the level
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));

	CurrentLevelName = LevelName;
	bLevelLoaded = true;

	InitializeLevelData();

	OnLevelLoaded.Broadcast(LevelName);

	UE_LOG(LogTemp, Log, TEXT("Level loaded: %s"), *LevelName);
	return true;
}

bool ULevelManager::GenerateProceduralLevel(int32 Seed)
{
	if (!LevelGenerator)
	{
		// Spawn level generator
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		LevelGenerator = GetWorld()->SpawnActor<ALevelGenerator>(ALevelGenerator::StaticClass(), SpawnParams);
	}

	if (LevelGenerator)
	{
		// Unload current level first
		if (bLevelLoaded)
		{
			UnloadCurrentLevel();
		}

		// Generate new level
		LevelGenerator->GenerateLevel(Seed);

		CurrentLevelName = FString::Printf(TEXT("Procedural_%d"), Seed);
		bLevelLoaded = true;

		InitializeLevelData();

		OnLevelLoaded.Broadcast(CurrentLevelName);

		UE_LOG(LogTemp, Log, TEXT("Procedural level generated with seed: %d"), Seed);
		return true;
	}

	return false;
}

void ULevelManager::UnloadCurrentLevel()
{
	if (!bLevelLoaded)
	{
		return;
	}

	CleanupLevelData();

	if (LevelGenerator)
	{
		LevelGenerator->Destroy();
		LevelGenerator = nullptr;
	}

	OnLevelUnloaded.Broadcast(CurrentLevelName);

	CurrentLevelName.Empty();
	bLevelLoaded = false;

	UE_LOG(LogTemp, Log, TEXT("Level unloaded"));
}

FString ULevelManager::GetCurrentLevelName() const
{
	return CurrentLevelName;
}

bool ULevelManager::IsLevelLoaded() const
{
	return bLevelLoaded;
}

TArray<FVector> ULevelManager::GetSpawnPoints() const
{
	return SpawnPoints;
}

TArray<FVector> ULevelManager::GetObjectiveLocations() const
{
	return ObjectiveLocations;
}

void ULevelManager::TransitionToLevel(const FString& LevelName)
{
	OnLevelTransition.Broadcast();
	LoadLevel(LevelName);
}

void ULevelManager::RestartCurrentLevel()
{
	if (!CurrentLevelName.IsEmpty())
	{
		LoadLevel(CurrentLevelName);
	}
}

void ULevelManager::SetDifficulty(float Difficulty)
{
	CurrentDifficulty = FMath::Max(0.1f, Difficulty);

	if (LevelGenerator)
	{
		LevelGenerator->SetDifficulty(CurrentDifficulty);
	}

	UE_LOG(LogTemp, Log, TEXT("Level difficulty set to: %f"), CurrentDifficulty);
}

void ULevelManager::InitializeLevelData()
{
	SpawnPoints.Empty();
	ObjectiveLocations.Empty();

	// Find spawn points in the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->ActorHasTag(TEXT("SpawnPoint")))
		{
			SpawnPoints.Add(Actor->GetActorLocation());
		}
		else if (Actor->ActorHasTag(TEXT("Objective")))
		{
			ObjectiveLocations.Add(Actor->GetActorLocation());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Level initialized with %d spawn points and %d objectives"),
		SpawnPoints.Num(), ObjectiveLocations.Num());
}

void ULevelManager::CleanupLevelData()
{
	SpawnPoints.Empty();
	ObjectiveLocations.Empty();
}

bool ULevelManager::ValidateLevelData() const
{
	// Basic validation
	return SpawnPoints.Num() > 0 && ObjectiveLocations.Num() > 0;
}