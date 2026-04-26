#include "Multiplayer/MultiplayerFPSGameSession.h"
#include "Player/MultiplayerFPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AMultiplayerFPSGameSession::AMultiplayerFPSGameSession()
{
	bGameInProgress = false;
	MaxPlayers = 8;
}

void AMultiplayerFPSGameSession::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Game session started"));
}

void AMultiplayerFPSGameSession::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up all players
	for (AMultiplayerFPSPlayerController* Player : ConnectedPlayers)
	{
		if (Player)
		{
			UnregisterPlayer(Player);
		}
	}
	ConnectedPlayers.Empty();

	Super::EndPlay(EndPlayReason);
}

bool AMultiplayerFPSGameSession::CreateRoom(const FString& InRoomName, int32 InMaxPlayers)
{
	if (InRoomName.IsEmpty() || InMaxPlayers <= 0)
	{
		return false;
	}

	RoomName = InRoomName;
	MaxPlayers = InMaxPlayers;
	bGameInProgress = false;

	UE_LOG(LogTemp, Log, TEXT("Room created: %s (Max players: %d)"), *RoomName, MaxPlayers);
	return true;
}

bool AMultiplayerFPSGameSession::JoinRoom(const FString& InRoomName, AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController || InRoomName != RoomName || IsRoomFull() || bGameInProgress)
	{
		return false;
	}

	RegisterPlayer(PlayerController);
	return true;
}

bool AMultiplayerFPSGameSession::LeaveRoom(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return false;
	}

	UnregisterPlayer(PlayerController);
	return true;
}

void AMultiplayerFPSGameSession::DestroyRoom()
{
	// Notify all players
	for (AMultiplayerFPSPlayerController* Player : ConnectedPlayers)
	{
		if (Player)
		{
			// Could send disconnect message
		}
	}

	ConnectedPlayers.Empty();
	RoomName.Empty();
	bGameInProgress = false;

	UE_LOG(LogTemp, Log, TEXT("Room destroyed"));
}

void AMultiplayerFPSGameSession::RegisterPlayer(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController || ConnectedPlayers.Contains(PlayerController))
	{
		return;
	}

	ConnectedPlayers.Add(PlayerController);
	OnPlayerJoined.Broadcast(PlayerController);

	BroadcastPlayerListUpdate();

	UE_LOG(LogTemp, Log, TEXT("Player joined room: %s"), *PlayerController->GetName());
}

void AMultiplayerFPSGameSession::UnregisterPlayer(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController || !ConnectedPlayers.Contains(PlayerController))
	{
		return;
	}

	ConnectedPlayers.Remove(PlayerController);
	OnPlayerLeft.Broadcast(PlayerController);

	BroadcastPlayerListUpdate();

	UE_LOG(LogTemp, Log, TEXT("Player left room: %s"), *PlayerController->GetName());

	// If no players left, destroy room
	if (ConnectedPlayers.Num() == 0)
	{
		DestroyRoom();
	}
}

TArray<AMultiplayerFPSPlayerController*> AMultiplayerFPSGameSession::GetConnectedPlayers() const
{
	return ConnectedPlayers;
}

void AMultiplayerFPSGameSession::StartGame()
{
	if (ConnectedPlayers.Num() < 2 || bGameInProgress)
	{
		return;
	}

	bGameInProgress = true;
	OnGameStarted.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Game started with %d players"), ConnectedPlayers.Num());
}

void AMultiplayerFPSGameSession::EndGame()
{
	if (!bGameInProgress)
	{
		return;
	}

	bGameInProgress = false;
	OnGameEnded.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Game ended"));
}

void AMultiplayerFPSGameSession::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerFPSGameSession, RoomName);
	DOREPLIFETIME(AMultiplayerFPSGameSession, MaxPlayers);
	DOREPLIFETIME(AMultiplayerFPSGameSession, bGameInProgress);
}

void AMultiplayerFPSGameSession::BroadcastPlayerListUpdate()
{
	// Notify all connected players of the updated player list
	for (AMultiplayerFPSPlayerController* Player : ConnectedPlayers)
	{
		if (Player)
		{
			// Could call a function on the player controller to update UI
		}
	}
}

void AMultiplayerFPSGameSession::HandlePlayerDisconnect(AMultiplayerFPSPlayerController* PlayerController)
{
	if (PlayerController)
	{
		UnregisterPlayer(PlayerController);
	}
}