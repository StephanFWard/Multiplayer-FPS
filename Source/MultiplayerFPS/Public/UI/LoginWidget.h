#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * Login widget for connecting to multiplayer sessions
 */
UCLASS()
class MULTIPLAYERFPS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULoginWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// Login functions
	UFUNCTION(BlueprintCallable, Category = "Login")
	void OnLoginButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void OnCreateRoomButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void OnJoinRoomButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void UpdateRoomList(TArray<FString> RoomNames);

protected:
	// UI Components
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PlayerNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* RoomNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateRoomButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinRoomButton;

	UPROPERTY(meta = (BindWidget))
	class UListView* RoomListView;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatusText;

private:
	FString PlayerName;
	FString SelectedRoomName;
};