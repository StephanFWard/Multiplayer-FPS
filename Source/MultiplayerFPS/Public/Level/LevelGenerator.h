#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

class UInstancedStaticMeshComponent;

/**
 * Generates procedural levels with rooms, corridors, and objectives
 */
UCLASS()
class MULTIPLAYERFPS_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	ALevelGenerator();

	virtual void BeginPlay() override;

	// Generation
	void GenerateLevel(int32 Seed = 0);
	void ClearLevel();

	// Settings
	void SetDifficulty(float Difficulty) { CurrentDifficulty = Difficulty; }
	float GetDifficulty() const { return CurrentDifficulty; }

	// Level data access
	TArray<FVector> GetSpawnPoints() const { return SpawnPoints; }
	TArray<FVector> GetObjectiveLocations() const { return ObjectiveLocations; }

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* FloorMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* WallMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* CoverMeshes;

	// Generation settings
	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 GridSize = 20;

	UPROPERTY(EditAnywhere, Category = "Generation")
	float CellSize = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MinRooms = 3;

	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MaxRooms = 8;

	UPROPERTY(EditAnywhere, Category = "Generation")
	float Difficulty = 1.0f;

	// Level data
	TArray<FVector> SpawnPoints;
	TArray<FVector> ObjectiveLocations;
	float CurrentDifficulty;

private:
	// Generation algorithms
	void GenerateRoomLayout(int32 Seed);
	void GenerateCorridors();
	void PlaceObjectives();
	void PlaceSpawnPoints();
	void GenerateGeometry();

	// Helper functions
	FVector GridToWorld(int32 X, int32 Y) const;
	bool IsValidGridPosition(int32 X, int32 Y) const;
	void AddFloorTile(int32 X, int32 Y);
	void AddWallTile(int32 X, int32 Y);
	void AddCoverTile(int32 X, int32 Y);

	// Grid data
	TArray<TArray<bool>> Grid; // true = occupied
};