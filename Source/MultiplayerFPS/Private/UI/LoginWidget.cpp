#include "UI/LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

ULoginWidget::ULoginWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button events
	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginButtonClicked);
	}

	if (CreateRoomButton)
	{
		CreateRoomButton->OnClicked.AddDynamic(this, &ULoginWidget::OnCreateRoomButtonClicked);
	}

	if (JoinRoomButton)
	{
		JoinRoomButton->OnClicked.AddDynamic(this, &ULoginWidget::OnJoinRoomButtonClicked);
	}

	// Set default player name
	if (PlayerNameTextBox)
	{
		PlayerNameTextBox->SetText(FText::FromString("Player"));
	}

	if (StatusText)
	{
		StatusText->SetText(FText::FromString("Enter your name and select a room"));
	}
}

void ULoginWidget::OnLoginButtonClicked()
{
	if (PlayerNameTextBox)
	{
		PlayerName = PlayerNameTextBox->GetText().ToString();
		UE_LOG(LogTemp, Warning, TEXT("Player logged in: %s"), *PlayerName);
	}

	if (StatusText)
	{
		StatusText->SetText(FText::FromString("Logged in! Select or create a room."));
	}
}

void ULoginWidget::OnCreateRoomButtonClicked()
{
	if (RoomNameTextBox)
	{
		FString RoomName = RoomNameTextBox->GetText().ToString();
		if (!RoomName.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating room: %s"), *RoomName);

			if (StatusText)
			{
				StatusText->SetText(FText::FromString("Creating room..."));
			}
		}
	}
}

void ULoginWidget::OnJoinRoomButtonClicked()
{
	if (!SelectedRoomName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining room: %s"), *SelectedRoomName);

		if (StatusText)
		{
			StatusText->SetText(FText::FromString("Joining room..."));
		}
	}
}

void ULoginWidget::UpdateRoomList(TArray<FString> RoomNames)
{
	if (RoomListView)
	{
		// Clear existing items
		RoomListView->ClearListItems();

		// Add new items (simplified - in practice you'd create list item widgets)
		for (const FString& RoomName : RoomNames)
		{
			// For now, just log
			UE_LOG(LogTemp, Warning, TEXT("Available room: %s"), *RoomName);
		}
	}
}