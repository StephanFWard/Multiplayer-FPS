#include "Game/MultiplayerFPSPlayerController.h"
#include "Character/MultiplayerFPSCharacter.h"
#include "UI/MultiplayerFPSHUD.h"
#include "UI/ChatWidget.h"
#include "Blueprint/UserWidget.h"

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

	// Initialize UI
	ShowHUD();
}

void AMultiplayerFPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHUD();
}

void AMultiplayerFPSPlayerController::ShowHUD()
{
	if (!GameHUD)
	{
		// Create HUD widget
		if (UClass* HUDWidgetClass = LoadClass<UMultiplayerFPSHUD>(nullptr, TEXT("/Game/UI/W_HUD.W_HUD_C")))
		{
			GameHUD = CreateWidget<UMultiplayerFPSHUD>(this, HUDWidgetClass);
			if (GameHUD)
			{
				GameHUD->AddToViewport();
			}
		}
	}

	if (!ChatWidget)
	{
		// Create chat widget
		if (UClass* ChatWidgetClass = LoadClass<UChatWidget>(nullptr, TEXT("/Game/UI/W_Chat.W_Chat_C")))
		{
			ChatWidget = CreateWidget<UChatWidget>(this, ChatWidgetClass);
			if (ChatWidget)
			{
				ChatWidget->AddToViewport();
			}
		}
	}
}

void AMultiplayerFPSPlayerController::HideHUD()
{
	if (GameHUD)
	{
		GameHUD->RemoveFromViewport();
		GameHUD = nullptr;
	}

	if (ChatWidget)
	{
		ChatWidget->RemoveFromViewport();
		ChatWidget = nullptr;
	}
}

void AMultiplayerFPSPlayerController::ToggleChat()
{
	if (ChatWidget)
	{
		bool bIsVisible = ChatWidget->IsVisible();
		ChatWidget->ToggleChat(!bIsVisible);
	}
}

void AMultiplayerFPSPlayerController::UpdateHUD()
{
	if (!GameHUD || !ControlledCharacter)
	{
		return;
	}

	// Update health
	GameHUD->UpdateHealth(ControlledCharacter->GetHealth(), ControlledCharacter->MaxHealth);

	// Update ammo
	if (ControlledCharacter->CurrentWeapon)
	{
		GameHUD->UpdateAmmo(
			ControlledCharacter->CurrentWeapon->GetCurrentAmmo(),
			ControlledCharacter->CurrentWeapon->GetTotalAmmo()
		);
	}

	// Update crosshair
	GameHUD->UpdateCrosshair(ControlledCharacter->IsAiming());
}
