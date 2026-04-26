#include "Game/MultiplayerFPSPlayerController.h"
#include "Character/MultiplayerFPSCharacter.h"

AMultiplayerFPSPlayerController::AMultiplayerFPSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiplayerFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Get the controlled character
	ControlledCharacter = Cast<AMultiplayerFPSCharacter>(GetCharacter());
	if (ControlledCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player controller initialized with character: %s"), *ControlledCharacter->GetName());
	}
}

void AMultiplayerFPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
