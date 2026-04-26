#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DisconnectHandler.generated.h"

class AMultiplayerFPSPlayerController;

/**
 * Handles player disconnections and reconnection logic
 */
UCLASS()
class MULTIPLAYERFPS_API UDisconnectHandler : public UObject
{
	GENERATED_BODY()

public:
	UDisconnectHandler();

	// Disconnect handling
	void OnPlayerDisconnected(AMultiplayerFPSPlayerController* PlayerController);
	void OnPlayerReconnected(AMultiplayerFPSPlayerController* PlayerController);

	// Reconnection
	bool CanPlayerReconnect(AMultiplayerFPSPlayerController* PlayerController) const;
	void StartReconnectionTimer(AMultiplayerFPSPlayerController* PlayerController);
	void CancelReconnectionTimer(AMultiplayerFPSPlayerController* PlayerController);

	// State management
	bool IsPlayerDisconnected(AMultiplayerFPSPlayerController* PlayerController) const;
	float GetReconnectionTimeRemaining(AMultiplayerFPSPlayerController* PlayerController) const;

	// Events
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerDisconnected, AMultiplayerFPSPlayerController*)
	FOnPlayerDisconnected OnPlayerDisconnectedEvent;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerReconnected, AMultiplayerFPSPlayerController*)
	FOnPlayerReconnected OnPlayerReconnectedEvent;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnReconnectionTimeout, AMultiplayerFPSPlayerController*)
	FOnReconnectionTimeout OnReconnectionTimeout;

protected:
	// Disconnected players tracking
	struct FDisconnectedPlayerInfo
	{
		AMultiplayerFPSPlayerController* PlayerController;
		float DisconnectTime;
		float ReconnectionTimer;
		bool bCanReconnect;

		FDisconnectedPlayerInfo()
			: PlayerController(nullptr), DisconnectTime(0.0f), ReconnectionTimer(0.0f), bCanReconnect(true)
		{}
	};

	TMap<AMultiplayerFPSPlayerController*, FDisconnectedPlayerInfo> DisconnectedPlayers;

	// Settings
	UPROPERTY(EditAnywhere, Category = "Reconnection")
	float MaxReconnectionTime = 60.0f; // Seconds

	UPROPERTY(EditAnywhere, Category = "Reconnection")
	float ReconnectionGracePeriod = 10.0f; // Seconds before allowing reconnection

private:
	// Helper functions
	void RemoveDisconnectedPlayer(AMultiplayerFPSPlayerController* PlayerController);
	void UpdateReconnectionTimers(float DeltaTime);
};