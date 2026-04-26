#include "Game/MultiplayerFPSGameMode.h"

AMultiplayerFPSGameMode::AMultiplayerFPSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxPlayers = 8;
	RespawnDelay = 5.0f;
	RoundTime = 600.0f;
}

void AMultiplayerFPSGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartGame();
}

void AMultiplayerFPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update round time
	RoundTime -= DeltaTime;
	
	// Check if round time is over
	if (RoundTime <= 0.0f)
	{
		EndGame();
	}
}

void AMultiplayerFPSGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Started!"));
	CurrentPlayerCount = GetNumPlayers();
}

void AMultiplayerFPSGameMode::EndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Ended!"));
}

void AMultiplayerFPSGameMode::RespawnPlayer(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	// TODO: Implement player respawn logic
	// This will teleport player to a spawn point and reset their health/state
}
