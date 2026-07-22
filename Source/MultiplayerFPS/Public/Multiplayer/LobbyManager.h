#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LobbyManager.generated.h"

class AMultiplayerFPSGameSession;
class AMultiplayerFPSPlayerController;

/**
 * Manages lobby creation, joining, and room discovery
 */
UCLASS()
class MULTIPLAYERFPS_API ULobbyManager : public UObject
{
	GENERATED_BODY()

public:
	ULobbyManager();

	// Lobby operations
	bool CreateLobby(const FString& LobbyName, int32 MaxPlayers = 8);
	bool JoinLobby(const FString& LobbyName);
	bool LeaveLobby();
	void RefreshAvailableLobbies();

	// Lobby information
	TArray<FString> GetAvailableLobbies() const;
	FString GetCurrentLobbyName() const;
	int32 GetCurrentLobbyPlayerCount() const;
	int32 GetCurrentLobbyMaxPlayers() const;
	bool IsInLobby() const { return CurrentSession != nullptr; }

	// Game start
	bool StartGame();

	// Events
	DECLARE_MULTICAST_DELEGATE(FOnLobbyCreated)
	FOnLobbyCreated OnLobbyCreated;

	DECLARE_MULTICAST_DELEGATE(FOnLobbyJoined)
	FOnLobbyJoined OnLobbyJoined;

	DECLARE_MULTICAST_DELEGATE(FOnLobbyLeft)
	FOnLobbyLeft OnLobbyLeft;

	DECLARE_MULTICAST_DELEGATE(FOnLobbiesRefreshed)
	FOnLobbiesRefreshed OnLobbiesRefreshed;

	DECLARE_MULTICAST_DELEGATE(FOnGameStarting)
	FOnGameStarting OnGameStarting;

protected:
	// Current session
	UPROPERTY()
	AMultiplayerFPSGameSession* CurrentSession;

	// Available lobbies (simulated for now)
	UPROPERTY()
	TArray<FString> AvailableLobbies;

private:
	// Helper functions
	void UpdateAvailableLobbies();
	void OnSessionPlayerJoined(AMultiplayerFPSPlayerController* Player);
	void OnSessionPlayerLeft(AMultiplayerFPSPlayerController* Player);
	void OnSessionGameStarted();
	void OnSessionGameEnded();
};
