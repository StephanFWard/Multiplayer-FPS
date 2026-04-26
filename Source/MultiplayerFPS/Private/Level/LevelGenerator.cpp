#include "Level/LevelGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ALevelGenerator::ALevelGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	// Create instanced mesh components
	FloorMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorMeshes"));
	FloorMeshes->SetupAttachment(RootComponent);

	WallMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WallMeshes"));
	WallMeshes->SetupAttachment(RootComponent);

	CoverMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CoverMeshes"));
	CoverMeshes->SetupAttachment(RootComponent);

	CurrentDifficulty = 1.0f;
}

void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Initialize grid
	Grid.SetNum(GridSize);
	for (int32 i = 0; i < GridSize; ++i)
	{
		Grid[i].SetNum(GridSize);
		for (int32 j = 0; j < GridSize; ++j)
		{
			Grid[i][j] = false;
		}
	}
}

void ALevelGenerator::GenerateLevel(int32 Seed)
{
	if (Seed == 0)
	{
		Seed = FMath::Rand();
	}

	// Seed random number generator
	FMath::RandInit(Seed);

	ClearLevel();

	GenerateRoomLayout(Seed);
	GenerateCorridors();
	PlaceObjectives();
	PlaceSpawnPoints();
	GenerateGeometry();

	UE_LOG(LogTemp, Log, TEXT("Level generated with seed: %d"), Seed);
}

void ALevelGenerator::ClearLevel()
{
	// Clear all instanced meshes
	if (FloorMeshes)
	{
		FloorMeshes->ClearInstances();
	}
	if (WallMeshes)
	{
		WallMeshes->ClearInstances();
	}
	if (CoverMeshes)
	{
		CoverMeshes->ClearInstances();
	}

	// Clear data
	SpawnPoints.Empty();
	ObjectiveLocations.Empty();

	// Reset grid
	for (int32 i = 0; i < GridSize; ++i)
	{
		for (int32 j = 0; j < GridSize; ++j)
		{
			Grid[i][j] = false;
		}
	}
}

void ALevelGenerator::GenerateRoomLayout(int32 Seed)
{
	int32 NumRooms = FMath::RandRange(MinRooms, MaxRooms);

	for (int32 i = 0; i < NumRooms; ++i)
	{
		int32 RoomWidth = FMath::RandRange(3, 6);
		int32 RoomHeight = FMath::RandRange(3, 6);
		int32 RoomX = FMath::RandRange(1, GridSize - RoomWidth - 1);
		int32 RoomY = FMath::RandRange(1, GridSize - RoomHeight - 1);

		// Place room
		for (int32 x = RoomX; x < RoomX + RoomWidth; ++x)
		{
			for (int32 y = RoomY; y < RoomY + RoomHeight; ++y)
			{
				if (IsValidGridPosition(x, y))
				{
					Grid[x][y] = true;
				}
			}
		}
	}
}

void ALevelGenerator::GenerateCorridors()
{
	// Simple corridor generation between rooms
	// This is a basic implementation - could be improved
	for (int32 x = 1; x < GridSize - 1; ++x)
	{
		for (int32 y = 1; y < GridSize - 1; ++y)
		{
			if (!Grid[x][y])
			{
				// Check if this position connects rooms
				int32 AdjacentRooms = 0;
				if (Grid[x-1][y]) AdjacentRooms++;
				if (Grid[x+1][y]) AdjacentRooms++;
				if (Grid[x][y-1]) AdjacentRooms++;
				if (Grid[x][y+1]) AdjacentRooms++;

				if (AdjacentRooms >= 2)
				{
					Grid[x][y] = true;
				}
			}
		}
	}
}

void ALevelGenerator::PlaceObjectives()
{
	ObjectiveLocations.Empty();

	// Place objectives in larger rooms
	for (int32 x = 1; x < GridSize - 1; ++x)
	{
		for (int32 y = 1; y < GridSize - 1; ++y)
		{
			if (Grid[x][y])
			{
				// Check if this is a room center
				int32 AdjacentWalls = 0;
				if (!Grid[x-1][y]) AdjacentWalls++;
				if (!Grid[x+1][y]) AdjacentWalls++;
				if (!Grid[x][y-1]) AdjacentWalls++;
				if (!Grid[x][y+1]) AdjacentWalls++;

				if (AdjacentWalls <= 2 && FMath::RandRange(0, 10) < 2) // 20% chance
				{
					ObjectiveLocations.Add(GridToWorld(x, y));
				}
			}
		}
	}

	// Ensure at least one objective
	if (ObjectiveLocations.Num() == 0 && GridSize > 0)
	{
		ObjectiveLocations.Add(GridToWorld(GridSize / 2, GridSize / 2));
	}
}

void ALevelGenerator::PlaceSpawnPoints()
{
	SpawnPoints.Empty();

	// Place spawn points at room entrances
	for (int32 x = 1; x < GridSize - 1; ++x)
	{
		for (int32 y = 1; y < GridSize - 1; ++y)
		{
			if (Grid[x][y])
			{
				// Check if adjacent to empty space (entrance)
				bool bIsEntrance = false;
				if (!Grid[x-1][y] || !Grid[x+1][y] || !Grid[x][y-1] || !Grid[x][y+1])
				{
					bIsEntrance = true;
				}

				if (bIsEntrance && FMath::RandRange(0, 5) == 0) // 20% chance
				{
					SpawnPoints.Add(GridToWorld(x, y));
				}
			}
		}
	}

	// Ensure at least 2 spawn points
	while (SpawnPoints.Num() < 2)
	{
		int32 x = FMath::RandRange(1, GridSize - 2);
		int32 y = FMath::RandRange(1, GridSize - 2);
		if (Grid[x][y])
		{
			SpawnPoints.Add(GridToWorld(x, y));
		}
	}
}

void ALevelGenerator::GenerateGeometry()
{
	for (int32 x = 0; x < GridSize; ++x)
	{
		for (int32 y = 0; y < GridSize; ++y)
		{
			if (Grid[x][y])
			{
				AddFloorTile(x, y);

				// Add walls around edges
				if (!Grid[x-1][y] && x > 0) AddWallTile(x-1, y);
				if (!Grid[x+1][y] && x < GridSize-1) AddWallTile(x+1, y);
				if (!Grid[x][y-1] && y > 0) AddWallTile(x, y-1);
				if (!Grid[x][y+1] && y < GridSize-1) AddWallTile(x, y+1);

				// Add cover randomly
				if (FMath::RandRange(0, 10) < 3) // 30% chance
				{
					AddCoverTile(x, y);
				}
			}
		}
	}
}

FVector ALevelGenerator::GridToWorld(int32 X, int32 Y) const
{
	return FVector(X * CellSize, Y * CellSize, 0.0f);
}

bool ALevelGenerator::IsValidGridPosition(int32 X, int32 Y) const
{
	return X >= 0 && X < GridSize && Y >= 0 && Y < GridSize;
}

void ALevelGenerator::AddFloorTile(int32 X, int32 Y)
{
	if (FloorMeshes)
	{
		FTransform Transform;
		Transform.SetLocation(GridToWorld(X, Y));
		FloorMeshes->AddInstance(Transform);
	}
}

void ALevelGenerator::AddWallTile(int32 X, int32 Y)
{
	if (WallMeshes)
	{
		FTransform Transform;
		Transform.SetLocation(GridToWorld(X, Y) + FVector(0, 0, 100.0f));
		WallMeshes->AddInstance(Transform);
	}
}

void ALevelGenerator::AddCoverTile(int32 X, int32 Y)
{
	if (CoverMeshes)
	{
		FTransform Transform;
		Transform.SetLocation(GridToWorld(X, Y) + FVector(0, 0, 50.0f));
		CoverMeshes->AddInstance(Transform);
	}
}