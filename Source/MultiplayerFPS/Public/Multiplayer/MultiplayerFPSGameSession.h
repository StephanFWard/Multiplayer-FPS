#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MultiplayerFPSGameSession.generated.h"

class AMultiplayerFPSPlayerController;

/**
 * Custom game session for managing multiplayer rooms and player connections
 */
UCLASS()
class MULTIPLAYERFPS_API AMultiplayerFPSGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	AMultiplayerFPSGameSession();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Session management
	bool CreateRoom(const FString& RoomName, int32 MaxPlayers = 8);
	bool JoinRoom(const FString& RoomName, AMultiplayerFPSPlayerController* PlayerController);
	bool LeaveRoom(AMultiplayerFPSPlayerController* PlayerController);
	void DestroyRoom();

	// Player management
	void RegisterPlayer(AMultiplayerFPSPlayerController* PlayerController);
	void UnregisterPlayer(AMultiplayerFPSPlayerController* PlayerController);
	TArray<AMultiplayerFPSPlayerController*> GetConnectedPlayers() const;

	// Room information
	FString GetRoomName() const { return RoomName; }
	int32 GetMaxPlayers() const { return MaxPlayers; }
	int32 GetCurrentPlayerCount() const { return ConnectedPlayers.Num(); }
	bool IsRoomFull() const { return GetCurrentPlayerCount() >= MaxPlayers; }

	// Game rules
	void StartGame();
	void EndGame();
	bool IsGameInProgress() const { return bGameInProgress; }

	// Events
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, AMultiplayerFPSPlayerController*)
	FOnPlayerJoined OnPlayerJoined;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLeft, AMultiplayerFPSPlayerController*)
	FOnPlayerLeft OnPlayerLeft;

	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStarted;

	DECLARE_MULTICAST_DELEGATE(FOnGameEnded)
	FOnGameEnded OnGameEnded;

protected:
	// Room properties
	UPROPERTY(Replicated)
	FString RoomName;

	UPROPERTY(Replicated)
	int32 MaxPlayers;

	UPROPERTY(Replicated)
	bool bGameInProgress;

	// Connected players
	UPROPERTY()
	TArray<AMultiplayerFPSPlayerController*> ConnectedPlayers;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Helper functions
	void BroadcastPlayerListUpdate();
	void HandlePlayerDisconnect(AMultiplayerFPSPlayerController* PlayerController);
};