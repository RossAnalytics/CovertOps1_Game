// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMatchmakingSubsystem.h"
#include "CovertOps1.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"

void UCOMatchmakingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
	{
		SessionInterface = OSS->GetSessionInterface();
	}

	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCOMatchmakingSubsystem::OnCreateSessionComplete);
		SessionInterface->OnStartSessionCompleteDelegates.AddUObject(this, &UCOMatchmakingSubsystem::OnStartSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCOMatchmakingSubsystem::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCOMatchmakingSubsystem::OnJoinSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCOMatchmakingSubsystem::OnDestroySessionComplete);
	}
}

void UCOMatchmakingSubsystem::Deinitialize()
{
	SessionSearch.Reset();
	LastRawResults.Empty();
	LastSearchResults.Empty();
	SessionInterface.Reset();
	Super::Deinitialize();
}

void UCOMatchmakingSubsystem::HostOnlineSession(FName SessionName, int32 MaxPlayers, bool bIsLAN, ECovertOpsMatchMode Mode)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionHosted.Broadcast(false, FString(TEXT("Session interface unavailable")));
		return;
	}

	FOnlineSessionSettings Settings;
	Settings.NumPublicConnections = FMath::Max(2, MaxPlayers);
	Settings.NumPrivateConnections = 0;
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bUsesPresence = true;
	Settings.bUseLobbiesIfAvailable = true;
	Settings.bIsLANMatch = bIsLAN;
	Settings.bIsDedicated = false;
	Settings.Set(FName(TEXT("CO_MODE")), MatchModeToTag(Mode), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (SessionInterface->GetNamedSession(SessionName))
	{
		SessionInterface->DestroySession(SessionName);
	}

	const bool bRequested = SessionInterface->CreateSession(0, SessionName, Settings);
	if (!bRequested)
	{
		OnSessionHosted.Broadcast(false, FString(TEXT("CreateSession request failed")));
	}
}

void UCOMatchmakingSubsystem::FindOnlineSessions(int32 MaxResults, bool bIsLAN)
{
	if (!SessionInterface.IsValid())
	{
		LastSearchResults.Empty();
		OnSessionsFound.Broadcast(false, LastSearchResults);
		return;
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->bIsLanQuery = bIsLAN;
	SessionSearch->MaxSearchResults = FMath::Max(1, MaxResults);
	SessionSearch->PingBucketSize = 50;
	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		LastSearchResults.Empty();
		OnSessionsFound.Broadcast(false, LastSearchResults);
	}
}

void UCOMatchmakingSubsystem::JoinOnlineSessionById(const FString& SessionId)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionJoined.Broadcast(false, FString(TEXT("Session interface unavailable")));
		return;
	}

	for (const FOnlineSessionSearchResult& Result : LastRawResults)
	{
		if (Result.GetSessionIdStr() == SessionId)
		{
			if (!SessionInterface->JoinSession(0, NAME_GameSession, Result))
			{
				OnSessionJoined.Broadcast(false, FString(TEXT("JoinSession request failed")));
			}
			return;
		}
	}

	OnSessionJoined.Broadcast(false, FString(TEXT("Session ID not found in cached results")));
}

void UCOMatchmakingSubsystem::DestroyOnlineSession(FName SessionName)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionDestroyed.Broadcast(false, FString(TEXT("Session interface unavailable")));
		return;
	}

	if (!SessionInterface->DestroySession(SessionName))
	{
		OnSessionDestroyed.Broadcast(false, FString(TEXT("DestroySession request failed")));
	}
}

void UCOMatchmakingSubsystem::DestroyPrimaryGameSession()
{
	DestroyOnlineSession(NAME_GameSession);
}

bool UCOMatchmakingSubsystem::HasSessionInterface() const
{
	return SessionInterface.IsValid();
}

void UCOMatchmakingSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		OnSessionHosted.Broadcast(false, FString::Printf(TEXT("CreateSession failed for %s"), *SessionName.ToString()));
		return;
	}

	if (!SessionInterface->StartSession(SessionName))
	{
		OnSessionHosted.Broadcast(false, FString(TEXT("Session created but StartSession failed")));
	}
}

void UCOMatchmakingSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	OnSessionHosted.Broadcast(
		bWasSuccessful,
		bWasSuccessful
			? FString::Printf(TEXT("Session started: %s"), *SessionName.ToString())
			: FString::Printf(TEXT("StartSession failed: %s"), *SessionName.ToString()));
}

void UCOMatchmakingSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	LastRawResults.Empty();
	LastSearchResults.Empty();

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		OnSessionsFound.Broadcast(false, LastSearchResults);
		return;
	}

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		LastRawResults.Add(Result);

		FCOMatchmakingSearchResult OutResult;
		OutResult.SessionId = Result.GetSessionIdStr();
		OutResult.HostName = Result.Session.OwningUserName;
		OutResult.PingInMs = Result.PingInMs;
		OutResult.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		OutResult.CurrentPlayers = OutResult.MaxPlayers - Result.Session.NumOpenPublicConnections;
		Result.Session.SessionSettings.Get(FName(TEXT("CO_MODE")), OutResult.MatchModeTag);
		LastSearchResults.Add(OutResult);
	}

	OnSessionsFound.Broadcast(true, LastSearchResults);
}

void UCOMatchmakingSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		OnSessionJoined.Broadcast(false, FString(TEXT("Session interface unavailable")));
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		OnSessionJoined.Broadcast(false, FString::Printf(TEXT("Join session failed (%d)"), static_cast<int32>(Result)));
		return;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		OnSessionJoined.Broadcast(false, FString(TEXT("Join succeeded but no connect string")));
		return;
	}

	if (UGameInstance* GI = this->GetGameInstance())
	{
		if (APlayerController* PC = GI->GetFirstLocalPlayerController())
		{
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
			OnSessionJoined.Broadcast(true, FString::Printf(TEXT("Joining session: %s"), *ConnectString));
			return;
		}
	}

	OnSessionJoined.Broadcast(false, FString(TEXT("Join succeeded but no local player controller found")));
}

void UCOMatchmakingSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	OnSessionDestroyed.Broadcast(
		bWasSuccessful,
		bWasSuccessful
			? FString::Printf(TEXT("Destroyed session: %s"), *SessionName.ToString())
			: FString::Printf(TEXT("Failed destroying session: %s"), *SessionName.ToString()));
}

FString UCOMatchmakingSubsystem::MatchModeToTag(ECovertOpsMatchMode Mode) const
{
	switch (Mode)
	{
	case ECovertOpsMatchMode::CTF:
		return TEXT("CTF");
	case ECovertOpsMatchMode::SnD:
		return TEXT("SnD");
	case ECovertOpsMatchMode::TDM:
	default:
		return TEXT("TDM");
	}
}
