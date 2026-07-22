#include "Multiplayer/MultiplayerFPSGameSession.h"
#include "Game/MultiplayerFPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AMultiplayerFPSGameSession::AMultiplayerFPSGameSession()
{
	bGameInProgress = false;
	RoomMaxPlayers = 8;
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
	RoomMaxPlayers = InMaxPlayers;
	bGameInProgress = false;

	UE_LOG(LogTemp, Log, TEXT("Room created: %s (Max players: %d)"), *RoomName, RoomMaxPlayers);
	return true;
}

bool AMultiplayerFPSGameSession::JoinRoom(const FString& InRoomName, AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController || InRoomName != RoomName || IsRoomFull() || bGameInProgress)
	{
		return false;
	}

	// Create a dummy unique net ID for the player
	FUniqueNetIdRepl UniqueId;
	RegisterPlayer(PlayerController, UniqueId, false);
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

void AMultiplayerFPSGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);

	AMultiplayerFPSPlayerController* FPSPlayerController = Cast<AMultiplayerFPSPlayerController>(NewPlayer);
	if (FPSPlayerController && !ConnectedPlayers.Contains(FPSPlayerController))
	{
		ConnectedPlayers.Add(FPSPlayerController);
		OnPlayerJoined.Broadcast(FPSPlayerController);
		BroadcastPlayerListUpdate();

		UE_LOG(LogTemp, Log, TEXT("Player joined room: %s"), *FPSPlayerController->GetName());
	}
}

void AMultiplayerFPSGameSession::UnregisterPlayer(const APlayerController* ExitingPlayer)
{
	Super::UnregisterPlayer(ExitingPlayer);

	AMultiplayerFPSPlayerController* FPSPlayerController = Cast<AMultiplayerFPSPlayerController>(const_cast<APlayerController*>(ExitingPlayer));
	if (FPSPlayerController && ConnectedPlayers.Contains(FPSPlayerController))
	{
		ConnectedPlayers.Remove(FPSPlayerController);
		OnPlayerLeft.Broadcast(FPSPlayerController);
		BroadcastPlayerListUpdate();

		UE_LOG(LogTemp, Log, TEXT("Player left room: %s"), *FPSPlayerController->GetName());

		// If no players left, destroy room
		if (ConnectedPlayers.Num() == 0)
		{
			DestroyRoom();
		}
	}
}

void AMultiplayerFPSGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
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
	DOREPLIFETIME(AMultiplayerFPSGameSession, RoomMaxPlayers);
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
