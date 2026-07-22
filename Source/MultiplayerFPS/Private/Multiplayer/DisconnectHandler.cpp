#include "Multiplayer/DisconnectHandler.h"
#include "Game/MultiplayerFPSPlayerController.h"
#include "TimerManager.h"

UDisconnectHandler::UDisconnectHandler()
{
}

void UDisconnectHandler::OnPlayerDisconnected(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	FDisconnectedPlayerInfo& Info = DisconnectedPlayers.FindOrAdd(PlayerController);
	Info.PlayerController = PlayerController;
	Info.DisconnectTime = GetWorld()->GetTimeSeconds();
	Info.ReconnectionTimer = MaxReconnectionTime;
	Info.bCanReconnect = true;

	OnPlayerDisconnectedEvent.Broadcast(PlayerController);

	UE_LOG(LogTemp, Warning, TEXT("Player disconnected: %s"), *PlayerController->GetName());

	// Start reconnection timer
	StartReconnectionTimer(PlayerController);
}

void UDisconnectHandler::OnPlayerReconnected(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	if (DisconnectedPlayers.Contains(PlayerController))
	{
		// Cancel reconnection timer
		CancelReconnectionTimer(PlayerController);

		// Remove from disconnected list
		DisconnectedPlayers.Remove(PlayerController);

		OnPlayerReconnectedEvent.Broadcast(PlayerController);

		UE_LOG(LogTemp, Log, TEXT("Player reconnected: %s"), *PlayerController->GetName());
	}
}

bool UDisconnectHandler::CanPlayerReconnect(AMultiplayerFPSPlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		return false;
	}

	const FDisconnectedPlayerInfo* Info = DisconnectedPlayers.Find(PlayerController);
	if (!Info)
	{
		return false;
	}

	float TimeSinceDisconnect = GetWorld()->GetTimeSeconds() - Info->DisconnectTime;
	return Info->bCanReconnect && TimeSinceDisconnect <= MaxReconnectionTime;
}

void UDisconnectHandler::StartReconnectionTimer(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController || !GetWorld())
	{
		return;
	}

	// Set up a timer to handle reconnection timeout
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &UDisconnectHandler::CancelReconnectionTimer, PlayerController),
		MaxReconnectionTime,
		false
	);

	// Store timer handle in player info if needed
}

void UDisconnectHandler::CancelReconnectionTimer(AMultiplayerFPSPlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	if (DisconnectedPlayers.Contains(PlayerController))
	{
		FDisconnectedPlayerInfo& Info = DisconnectedPlayers[PlayerController];
		Info.bCanReconnect = false;

		OnReconnectionTimeout.Broadcast(PlayerController);

		UE_LOG(LogTemp, Warning, TEXT("Reconnection timeout for player: %s"), *PlayerController->GetName());

		// Remove player after timeout
		RemoveDisconnectedPlayer(PlayerController);
	}
}

bool UDisconnectHandler::IsPlayerDisconnected(AMultiplayerFPSPlayerController* PlayerController) const
{
	return DisconnectedPlayers.Contains(PlayerController);
}

float UDisconnectHandler::GetReconnectionTimeRemaining(AMultiplayerFPSPlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		return 0.0f;
	}

	const FDisconnectedPlayerInfo* Info = DisconnectedPlayers.Find(PlayerController);
	if (!Info)
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, Info->ReconnectionTimer - (GetWorld()->GetTimeSeconds() - Info->DisconnectTime));
}

void UDisconnectHandler::RemoveDisconnectedPlayer(AMultiplayerFPSPlayerController* PlayerController)
{
	if (PlayerController)
	{
		DisconnectedPlayers.Remove(PlayerController);

		// Notify game session to remove player
		// This would typically be handled by the game session
	}
}

void UDisconnectHandler::UpdateReconnectionTimers(float DeltaTime)
{
	TArray<AMultiplayerFPSPlayerController*> PlayersToRemove;

	for (auto& Pair : DisconnectedPlayers)
	{
		FDisconnectedPlayerInfo& Info = Pair.Value;
		Info.ReconnectionTimer -= DeltaTime;

		if (Info.ReconnectionTimer <= 0.0f)
		{
			PlayersToRemove.Add(Pair.Key);
		}
	}

	for (AMultiplayerFPSPlayerController* Player : PlayersToRemove)
	{
		CancelReconnectionTimer(Player);
	}
}