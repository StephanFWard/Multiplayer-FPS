#include "UI/ChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

UChatWidget::UChatWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button event
	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &UChatWidget::OnSendButtonClicked);
	}

	// Bind enter key for chat input
	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatTextCommitted);
	}

	// Initially hide chat
	SetVisibility(ESlateVisibility::Hidden);
}

void UChatWidget::SendMessage(const FString& Message)
{
	if (Message.IsEmpty())
	{
		return;
	}

	// Add to local history
	FString FullMessage = FString::Printf(TEXT("You: %s"), *Message);
	AddMessageToHistory(FullMessage);

	// Broadcast to other players (would use networking)
	UE_LOG(LogTemp, Warning, TEXT("Chat message sent: %s"), *Message);

	// Clear input
	if (ChatInputBox)
	{
		ChatInputBox->SetText(FText::GetEmpty());
	}
}

void UChatWidget::ReceiveMessage(const FString& Sender, const FString& Message)
{
	FString FullMessage = FString::Printf(TEXT("%s: %s"), *Sender, *Message);
	AddMessageToHistory(FullMessage);
}

void UChatWidget::ToggleChat(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	if (bShow && ChatInputBox)
	{
		ChatInputBox->SetKeyboardFocus();
	}
}

void UChatWidget::AddMessageToHistory(const FString& Message)
{
	ChatMessages.Add(Message);

	// Limit message history
	if (ChatMessages.Num() > MaxMessages)
	{
		ChatMessages.RemoveAt(0);
	}

	// Update UI
	if (ChatHistoryBox)
	{
		ChatHistoryBox->ClearChildren();

		for (const FString& Msg : ChatMessages)
		{
			UTextBlock* MessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (MessageText)
			{
				MessageText->SetText(FText::FromString(Msg));
				MessageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				ChatHistoryBox->AddChild(MessageText);
			}
		}

		// Scroll to bottom
		ChatHistoryBox->ScrollToEnd();
	}
}

void UChatWidget::OnSendButtonClicked()
{
	if (ChatInputBox)
	{
		FString Message = ChatInputBox->GetText().ToString();
		SendMessage(Message);
	}
}

void UChatWidget::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString Message = Text.ToString();
		SendMessage(Message);
	}
}