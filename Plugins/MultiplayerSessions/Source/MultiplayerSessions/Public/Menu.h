// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 numberOfPublicConnections = 4, FString typeOfMatch = FString(TEXT("FreeForAll")), FString lobbyPath = FString(TEXT("/Game/Levels/Lobby")));

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultipayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:

	UPROPERTY(meta = (BindWidget))
	UButton* m_HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* m_JoinButton;

	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	// The subsystem designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* m_MultiplayerSessionsSubsystem;

	int32 m_NumPublicConnections{ 4 };
	FString m_MatchType{ TEXT("FreeForAll") };
	FString m_PathToLobby{ TEXT("") };
};
