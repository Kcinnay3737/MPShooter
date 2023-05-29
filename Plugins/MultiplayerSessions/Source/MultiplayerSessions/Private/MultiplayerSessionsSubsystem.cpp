// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	m_CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	m_FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	m_JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	m_DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	m_StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		m_SessionInterface = subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 numPublicConnections, FString matchType)
{
	if (!m_SessionInterface.IsValid()) return;

	auto existingSession = m_SessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession != nullptr)
	{
		m_bCreateSessionOnDestroy = true;
		m_LastNumPublicConnections = numPublicConnections;
		m_LastMatchType = matchType;

		DestroySession();
	}

	// Store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	m_CreateSessionCompleteDelegateHandle = m_SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(m_CreateSessionCompleteDelegate);

	m_LastSessionSettings = MakeShareable(new FOnlineSessionSettings);
	m_LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	m_LastSessionSettings->NumPublicConnections = numPublicConnections;
	m_LastSessionSettings->bAllowJoinInProgress = true;
	m_LastSessionSettings->bAllowJoinViaPresence = true;
	m_LastSessionSettings->bShouldAdvertise = true;
	m_LastSessionSettings->bUsesPresence = true;
	// Rajouter car ca marchais pas
	m_LastSessionSettings->bUseLobbiesIfAvailable = true;
	m_LastSessionSettings->Set(FName("MatchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	m_LastSessionSettings->BuildUniqueId = 1;

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!m_SessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *m_LastSessionSettings))
	{
		m_SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(m_CreateSessionCompleteDelegateHandle);

		// Broadcast our own custom delegate
		m_MultiplayerOnCreateSessionComplete.Broadcast(false);
	}

}

void UMultiplayerSessionsSubsystem::FindSessions(int32 maxSearchResults)
{
	if (!m_SessionInterface.IsValid()) return;

	m_FindSessionsCompleteDelegateHandle = m_SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(m_FindSessionsCompleteDelegate);

	m_LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	m_LastSessionSearch->MaxSearchResults = maxSearchResults;
	m_LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	m_LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!m_SessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), m_LastSessionSearch.ToSharedRef()))
	{
		m_SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(m_FindSessionsCompleteDelegateHandle);

		m_MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionResult)
{
	if (!m_SessionInterface.IsValid())
	{
		m_MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	m_JoinSessionCompleteDelegateHandle = m_SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(m_JoinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!m_SessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionResult))
	{
		m_SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(m_JoinSessionCompleteDelegateHandle);

		m_MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!m_SessionInterface.IsValid())
	{
		m_MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	m_DestroySessionCompleteDelegateHandle = m_SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(m_DestroySessionCompleteDelegate);

	if (!m_SessionInterface->DestroySession(NAME_GameSession))
	{
		m_SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(m_DestroySessionCompleteDelegateHandle);
		m_MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (m_SessionInterface)
	{
		m_SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(m_CreateSessionCompleteDelegateHandle);
	}

	m_MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (m_SessionInterface)
	{
		m_SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(m_FindSessionsCompleteDelegateHandle);
	}

	if (m_LastSessionSearch->SearchResults.Num() <= 0)
	{
		m_MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	m_MultiplayerOnFindSessionsComplete.Broadcast(m_LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (m_SessionInterface)
	{
		m_SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(m_JoinSessionCompleteDelegateHandle);
	}

	m_MultiplayerOnJoinSessionComplete.Broadcast(result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (m_SessionInterface)
	{
		m_SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(m_DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful && m_bCreateSessionOnDestroy)
	{
		m_bCreateSessionOnDestroy = false;
		CreateSession(m_LastNumPublicConnections, m_LastMatchType);
	}
	m_MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool bWasSuccessful)
{
}
