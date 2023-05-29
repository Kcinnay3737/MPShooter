// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

//
// Declaring our own custom delegates for the Menu class to bind callbacks to
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	//
	// To handle session functionality. The Menu class will call these
	//
	void CreateSession(int32 numPublicConnections, FString matchType);
	void FindSessions(int32 maxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& sessionResult);
	void DestroySession();
	void StartSession();

	//
	// Our own custom delegates for the Menu class to bind callbacks to
	//
	FMultiplayerOnCreateSessionComplete m_MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete m_MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete m_MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete m_MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete m_MultiplayerOnStartSessionComplete;

protected:

	//
	// Internal callbacks for delegates we'll add to the Online Session Interface delegate list.
	// Thise don't need to be called outside this class
	//
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName sessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr m_SessionInterface;
	TSharedPtr<FOnlineSessionSettings> m_LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> m_LastSessionSearch;

	//
	// To add to the Online Session Interface delegate list.
	// We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	//
	FOnCreateSessionCompleteDelegate m_CreateSessionCompleteDelegate;
	FDelegateHandle m_CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate m_FindSessionsCompleteDelegate;
	FDelegateHandle m_FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate m_JoinSessionCompleteDelegate;
	FDelegateHandle m_JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate m_DestroySessionCompleteDelegate;
	FDelegateHandle m_DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate m_StartSessionCompleteDelegate;
	FDelegateHandle m_StartSessionCompleteDelegateHandle;

	bool m_bCreateSessionOnDestroy{ false };
	int32 m_LastNumPublicConnections;
	FString m_LastMatchType;
};
