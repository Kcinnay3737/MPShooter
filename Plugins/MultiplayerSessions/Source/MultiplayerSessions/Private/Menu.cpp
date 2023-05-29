// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"


void UMenu::MenuSetup(int32 numberOfPublicConnections, FString typeOfMatch, FString lobbyPath)
{
	m_NumPublicConnections = numberOfPublicConnections;
	m_MatchType = typeOfMatch;
	m_PathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		m_MultiplayerSessionsSubsystem = gameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (m_MultiplayerSessionsSubsystem)
	{
		m_MultiplayerSessionsSubsystem->m_MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		m_MultiplayerSessionsSubsystem->m_MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
		m_MultiplayerSessionsSubsystem->m_MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		m_MultiplayerSessionsSubsystem->m_MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		m_MultiplayerSessionsSubsystem->m_MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (m_HostButton)
	{
		m_HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (m_JoinButton)
	{
		m_JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();

	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(m_PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString("Failed to create session!"));
		}
		m_HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful)
{
	if (m_MultiplayerSessionsSubsystem == nullptr) return;

	for (auto result : sessionResults)
	{
		FString settingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);
		if (settingsValue == m_MatchType)
		{
			m_MultiplayerSessionsSubsystem->JoinSession(result);
			return;
		}
	}
	if (!bWasSuccessful || sessionResults.Num() == 0)
	{
		m_JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		IOnlineSessionPtr sessionInterface = subsystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			FString address;
			sessionInterface->GetResolvedConnectString(NAME_GameSession, address);

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (result != EOnJoinSessionCompleteResult::Success)
	{
		m_JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	m_HostButton->SetIsEnabled(false);
	if (m_MultiplayerSessionsSubsystem)
	{
		m_MultiplayerSessionsSubsystem->CreateSession(m_NumPublicConnections, m_MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	m_JoinButton->SetIsEnabled(false);
	if (m_MultiplayerSessionsSubsystem)
	{
		m_MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}
