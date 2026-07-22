#include "Multiplayer/GameRules.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "Game/MultiplayerFPSPlayerController.h"

UGameRules::UGameRules()
{
	bGameActive = false;
	bGamePaused = false;
}

bool UGameRules::CanPlayerJoinGame(AMultiplayerFPSPlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		return false;
	}

	// Check if game is active and not full
	return !bGameActive || PlayerScores.Num() < MaxPlayers;
}

bool UGameRules::CanPlayerLeaveGame(AMultiplayerFPSPlayerController* PlayerController) const
{
	return PlayerController != nullptr;
}

bool UGameRules::CanPlayerRespawn(AMultiplayerFPSCharacter* Player) const
{
	if (!Player || !bGameActive || bGamePaused)
	{
		return false;
	}

	// Check respawn timer - using a simple time check since we don't have GetLastDeathTime
	// In a real implementation, you'd track death time on the character
	return true; // Simplified for now
}

bool UGameRules::CanPlayerFireWeapon(AMultiplayerFPSCharacter* Player) const
{
	if (!Player || !bGameActive || bGamePaused)
	{
		return false;
	}

	// Check if player is alive
	return Player->GetHealth() > 0.0f;
}

void UGameRules::StartGame()
{
	if (bGameActive)
	{
		return;
	}

	bGameActive = true;
	bGamePaused = false;

	OnGameStarted.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Game rules: Game started"));
}

void UGameRules::EndGame()
{
	if (!bGameActive)
	{
		return;
	}

	bGameActive = false;
	bGamePaused = false;

	OnGameEnded.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("Game rules: Game ended"));
}

void UGameRules::PauseGame()
{
	if (!bGameActive || bGamePaused)
	{
		return;
	}

	bGamePaused = true;
	UE_LOG(LogTemp, Log, TEXT("Game rules: Game paused"));
}

void UGameRules::ResumeGame()
{
	if (!bGameActive || !bGamePaused)
	{
		return;
	}

	bGamePaused = false;
	UE_LOG(LogTemp, Log, TEXT("Game rules: Game resumed"));
}

void UGameRules::OnPlayerKilled(AMultiplayerFPSCharacter* Killer, AMultiplayerFPSCharacter* Victim)
{
	if (!Killer || !Victim || !bGameActive)
	{
		return;
	}

	// Award kill score
	AMultiplayerFPSPlayerController* KillerController = Cast<AMultiplayerFPSPlayerController>(Killer->GetController());
	if (KillerController)
	{
		AwardScore(KillerController, KillScore);
	}

	OnPlayerKilledEvent.Broadcast(Victim);

	UE_LOG(LogTemp, Log, TEXT("Player killed: %s killed %s"), *Killer->GetName(), *Victim->GetName());

	CheckWinConditions();
}

void UGameRules::OnObjectiveCompleted(AMultiplayerFPSPlayerController* Player, const FString& ObjectiveName)
{
	if (!Player || !bGameActive)
	{
		return;
	}

	AwardScore(Player, ObjectiveScore);

	UE_LOG(LogTemp, Log, TEXT("Objective completed: %s by %s"), *ObjectiveName, *Player->GetName());

	CheckWinConditions();
}

int32 UGameRules::GetPlayerScore(AMultiplayerFPSPlayerController* Player) const
{
	if (!Player)
	{
		return 0;
	}

	const int32* Score = PlayerScores.Find(Player);
	return Score ? *Score : 0;
}

bool UGameRules::ValidatePlayerAction(AMultiplayerFPSPlayerController* Player, const FString& Action) const
{
	if (!Player)
	{
		return false;
	}

	// Basic anti-cheat: check for action spam
	float CurrentTime = GetWorld()->GetTimeSeconds();
	const float* LastTime = LastActionTimes.Find(Player);

	if (LastTime)
	{
		float TimeDiff = CurrentTime - *LastTime;
		if (TimeDiff < 0.1f) // Actions too frequent
		{
			return false;
		}
	}

	return true;
}

void UGameRules::ReportSuspiciousActivity(AMultiplayerFPSPlayerController* Player, const FString& Activity)
{
	if (!Player)
	{
		return;
	}

	int32& Count = SuspiciousActivityCount.FindOrAdd(Player);
	Count++;

	UE_LOG(LogTemp, Warning, TEXT("Suspicious activity reported for %s: %s (Count: %d)"),
		*Player->GetName(), *Activity, Count);

	// Could implement banning logic here
}

bool UGameRules::IsPlayerValid(AMultiplayerFPSPlayerController* Player) const
{
	return Player != nullptr && Player->IsValidLowLevel();
}

void UGameRules::AwardScore(AMultiplayerFPSPlayerController* Player, int32 Points)
{
	if (!Player)
	{
		return;
	}

	int32& Score = PlayerScores.FindOrAdd(Player);
	Score += Points;

	UE_LOG(LogTemp, Log, TEXT("Score awarded: %s gained %d points (Total: %d)"),
		*Player->GetName(), Points, Score);
}

void UGameRules::CheckWinConditions()
{
	// Simple win condition: first to reach 100 points
	for (const auto& Pair : PlayerScores)
	{
		if (Pair.Value >= 100)
		{
			UE_LOG(LogTemp, Log, TEXT("Win condition met! Player %s wins with %d points"),
				*Pair.Key->GetName(), Pair.Value);
			EndGame();
			break;
		}
	}
}