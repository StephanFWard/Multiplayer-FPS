#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

/**
 * Chat widget for in-game messaging
 */
UCLASS()
class MULTIPLAYERFPS_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UChatWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// Chat functions
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendMessage(const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void ReceiveMessage(const FString& Sender, const FString& Message);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void ToggleChat(bool bShow);

protected:
	// UI Components
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ChatHistoryBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* SendButton;

private:
	TArray<FString> ChatMessages;
	const int32 MaxMessages = 50;

	void AddMessageToHistory(const FString& Message);
};