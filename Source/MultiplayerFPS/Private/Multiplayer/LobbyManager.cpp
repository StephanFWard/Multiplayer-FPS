#include "Multiplayer/LobbyManager.h"
#include "Multiplayer/MultiplayerFPSGameSession.h"
#include "Game/MultiplayerFPSPlayerController.h"
#include "Kismet/GameplayStatics.h"

ULobbyManager::ULobbyManager()
{
	CurrentSession = nullptr;
	UpdateAvailableLobbies();
}

bool ULobbyManager::CreateLobby(const FString& LobbyName, int32 MaxPlayers)
{
	if (LobbyName.IsEmpty() || IsInLobby())
	{
		return false;
	}

	// Find or create game session
	TArray<AActor*> FoundSessions;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMultiplayerFPSGameSession::StaticClass(), FoundSessions);

	AMultiplayerFPSGameSession* NewSession = nullptr;

	if (FoundSessions.Num() > 0)
	{
		NewSession = Cast<AMultiplayerFPSGameSession>(FoundSessions[0]);
	}
	else
	{
		// Spawn new session
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		NewSession = GetWorld()->SpawnActor<AMultiplayerFPSGameSession>(AMultiplayerFPSGameSession::StaticClass(), SpawnParams);
	}

	if (NewSession && NewSession->CreateRoom(LobbyName, MaxPlayers))
	{
		CurrentSession = NewSession;

		// Bind to session events
		CurrentSession->OnPlayerJoined.AddUObject(this, &ULobbyManager::OnSessionPlayerJoined);
		CurrentSession->OnPlayerLeft.AddUObject(this, &ULobbyManager::OnSessionPlayerLeft);
		CurrentSession->OnGameStarted.AddUObject(this, &ULobbyManager::OnSessionGameStarted);
		CurrentSession->OnGameEnded.AddUObject(this, &ULobbyManager::OnSessionGameEnded);

		OnLobbyCreated.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Lobby created: %s"), *LobbyName);
		return true;
	}

	return false;
}

bool ULobbyManager::JoinLobby(const FString& LobbyName)
{
	if (LobbyName.IsEmpty() || IsInLobby())
	{
		return false;
	}

	// Find session with matching name
	TArray<AActor*> FoundSessions;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMultiplayerFPSGameSession::StaticClass(), FoundSessions);

	for (AActor* Actor : FoundSessions)
	{
		AMultiplayerFPSGameSession* Session = Cast<AMultiplayerFPSGameSession>(Actor);
		if (Session && Session->GetRoomName() == LobbyName)
		{
			// Get local player controller
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			AMultiplayerFPSPlayerController* FPSPC = Cast<AMultiplayerFPSPlayerController>(PC);

			if (FPSPC && Session->JoinRoom(LobbyName, FPSPC))
			{
				CurrentSession = Session;

				// Bind to session events
				CurrentSession->OnPlayerJoined.AddUObject(this, &ULobbyManager::OnSessionPlayerJoined);
				CurrentSession->OnPlayerLeft.AddUObject(this, &ULobbyManager::OnSessionPlayerLeft);
				CurrentSession->OnGameStarted.AddUObject(this, &ULobbyManager::OnSessionGameStarted);
				CurrentSession->OnGameEnded.AddUObject(this, &ULobbyManager::OnSessionGameEnded);

				OnLobbyJoined.Broadcast();
				UE_LOG(LogTemp, Log, TEXT("Joined lobby: %s"), *LobbyName);
				return true;
			}
		}
	}

	return false;
}

bool ULobbyManager::LeaveLobby()
{
	if (!IsInLobby())
	{
		return false;
	}

	// Get local player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AMultiplayerFPSPlayerController* FPSPC = Cast<AMultiplayerFPSPlayerController>(PC);

	if (FPSPC)
	{
		CurrentSession->LeaveRoom(FPSPC);
	}

	// Unbind events
	CurrentSession->OnPlayerJoined.RemoveAll(this);
	CurrentSession->OnPlayerLeft.RemoveAll(this);
	CurrentSession->OnGameStarted.RemoveAll(this);
	CurrentSession->OnGameEnded.RemoveAll(this);

	CurrentSession = nullptr;
	OnLobbyLeft.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Left lobby"));
	return true;
}

void ULobbyManager::RefreshAvailableLobbies()
{
	UpdateAvailableLobbies();
	OnLobbiesRefreshed.Broadcast();
}

TArray<FString> ULobbyManager::GetAvailableLobbies() const
{
	return AvailableLobbies;
}

FString ULobbyManager::GetCurrentLobbyName() const
{
	return CurrentSession ? CurrentSession->GetRoomName() : FString();
}

int32 ULobbyManager::GetCurrentLobbyPlayerCount() const
{
	return CurrentSession ? CurrentSession->GetCurrentPlayerCount() : 0;
}

int32 ULobbyManager::GetCurrentLobbyMaxPlayers() const
{
	return CurrentSession ? CurrentSession->GetMaxPlayers() : 0;
}

bool ULobbyManager::StartGame()
{
	if (!IsInLobby() || CurrentSession->GetCurrentPlayerCount() < 2)
	{
		return false;
	}

	CurrentSession->StartGame();
	return true;
}

void ULobbyManager::UpdateAvailableLobbies()
{
	AvailableLobbies.Empty();

	// Find all active sessions
	TArray<AActor*> FoundSessions;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMultiplayerFPSGameSession::StaticClass(), FoundSessions);

	for (AActor* Actor : FoundSessions)
	{
		AMultiplayerFPSGameSession* Session = Cast<AMultiplayerFPSGameSession>(Actor);
		if (Session && !Session->IsRoomFull() && !Session->IsGameInProgress())
		{
			AvailableLobbies.Add(Session->GetRoomName());
		}
	}

	// Add some default lobbies for testing
	if (AvailableLobbies.Num() == 0)
	{
		AvailableLobbies.Add("Quick Match");
		AvailableLobbies.Add("Beginner Lobby");
		AvailableLobbies.Add("Pro Arena");
	}
}

void ULobbyManager::OnSessionPlayerJoined(AMultiplayerFPSPlayerController* Player)
{
	UE_LOG(LogTemp, Log, TEXT("Player joined session"));
}

void ULobbyManager::OnSessionPlayerLeft(AMultiplayerFPSPlayerController* Player)
{
	UE_LOG(LogTemp, Log, TEXT("Player left session"));
}

void ULobbyManager::OnSessionGameStarted()
{
	OnGameStarting.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Game starting"));
}

void ULobbyManager::OnSessionGameEnded()
{
	UE_LOG(LogTemp, Log, TEXT("Game ended"));
}