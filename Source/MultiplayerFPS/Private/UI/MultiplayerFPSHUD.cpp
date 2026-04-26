#include "UI/MultiplayerFPSHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Blueprint/WidgetTree.h"

UMultiplayerFPSHUD::UMultiplayerFPSHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMultiplayerFPSHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize UI elements
	if (HealthBar)
	{
		HealthBar->SetPercent(1.0f);
	}

	if (HealthText)
	{
		HealthText->SetText(FText::FromString("100/100"));
	}

	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString("30/300"));
	}

	if (CrosshairImage)
	{
		CrosshairImage->SetOpacity(1.0f);
	}

	// Hide damage indicators initially
	if (HitMarkerImage)
	{
		HitMarkerImage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (DamageIndicatorUp) DamageIndicatorUp->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorDown) DamageIndicatorDown->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorLeft) DamageIndicatorLeft->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorRight) DamageIndicatorRight->SetVisibility(ESlateVisibility::Hidden);
}

void UMultiplayerFPSHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMultiplayerFPSHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}

	if (HealthText)
	{
		FString HealthString = FString::Printf(TEXT("%.0f/%.0f"), CurrentHealth, MaxHealth);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UMultiplayerFPSHUD::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		FString AmmoString = FString::Printf(TEXT("%d/%d"), CurrentAmmo, MaxAmmo);
		AmmoText->SetText(FText::FromString(AmmoString));
	}
}

void UMultiplayerFPSHUD::UpdateCrosshair(bool bIsAiming)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetOpacity(bIsAiming ? 0.5f : 1.0f);
	}
}

void UMultiplayerFPSHUD::ShowHitMarker()
{
	if (HitMarkerImage)
	{
		HitMarkerImage->SetVisibility(ESlateVisibility::Visible);

		// Auto-hide after 0.2 seconds
		GetWorld()->GetTimerManager().SetTimer(
			HitMarkerTimerHandle,
			this,
			&UMultiplayerFPSHUD::HideHitMarker,
			0.2f,
			false
		);
	}
}

void UMultiplayerFPSHUD::ShowDamageIndicator(FVector DamageDirection)
{
	// Determine which indicator to show based on damage direction
	UImage* IndicatorToShow = nullptr;

	// Simplified direction calculation
	if (FMath::Abs(DamageDirection.X) > FMath::Abs(DamageDirection.Y))
	{
		// Left/Right
		IndicatorToShow = (DamageDirection.X > 0) ? DamageIndicatorRight : DamageIndicatorLeft;
	}
	else
	{
		// Up/Down
		IndicatorToShow = (DamageDirection.Y > 0) ? DamageIndicatorUp : DamageIndicatorDown;
	}

	if (IndicatorToShow)
	{
		IndicatorToShow->SetVisibility(ESlateVisibility::Visible);

		// Auto-hide after 1 second
		GetWorld()->GetTimerManager().SetTimer(
			DamageIndicatorTimerHandle,
			this,
			&UMultiplayerFPSHUD::HideDamageIndicators,
			1.0f,
			false
		);
	}
}

void UMultiplayerFPSHUD::UpdatePlayerList(TArray<FString> PlayerNames)
{
	if (!PlayerListBox)
	{
		return;
	}

	// Clear existing entries
	PlayerListBox->ClearChildren();

	// Add new entries
	for (const FString& PlayerName : PlayerNames)
	{
		UTextBlock* PlayerText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		if (PlayerText)
		{
			PlayerText->SetText(FText::FromString(PlayerName));
			PlayerListBox->AddChild(PlayerText);
		}
	}
}

void UMultiplayerFPSHUD::HideHitMarker()
{
	if (HitMarkerImage)
	{
		HitMarkerImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMultiplayerFPSHUD::HideDamageIndicators()
{
	if (DamageIndicatorUp) DamageIndicatorUp->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorDown) DamageIndicatorDown->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorLeft) DamageIndicatorLeft->SetVisibility(ESlateVisibility::Hidden);
	if (DamageIndicatorRight) DamageIndicatorRight->SetVisibility(ESlateVisibility::Hidden);
}