#include "Game/MultiplayerFPSGameMode.h"
#include "Character/MultiplayerFPSCharacter.h"

AMultiplayerFPSGameMode::AMultiplayerFPSGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	MaxPlayers = 8;
	RespawnDelay = 5.0f;
	RoundTime = 600.0f;

	// Set default pawn class
	DefaultPawnClass = AMultiplayerFPSCharacter::StaticClass();
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

	// Get the player's pawn
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	// Find a player start
	AActor* PlayerStart = ChoosePlayerStart(PlayerController);
	if (!PlayerStart)
	{
		return;
	}

	// Teleport player to spawn point
	PlayerPawn->SetActorLocation(PlayerStart->GetActorLocation());
	PlayerPawn->SetActorRotation(PlayerStart->GetActorRotation());

	// Reset player health and state
	if (AMultiplayerFPSCharacter* Character = Cast<AMultiplayerFPSCharacter>(PlayerPawn))
	{
		Character->CurrentHealth = Character->MaxHealth;
		// TODO: Reset weapon ammo, etc.
	}

	UE_LOG(LogTemp, Warning, TEXT("Player respawned at: %s"), *PlayerStart->GetActorLocation().ToString());
}
}
