// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMatchmakingSubsystem.generated.h"

USTRUCT(BlueprintType)
struct COVERTOPS1_API FCOMatchmakingSearchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SessionId;

	UPROPERTY(BlueprintReadOnly)
	FString HostName;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PingInMs = 0;

	UPROPERTY(BlueprintReadOnly)
	FString MatchModeTag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCOMatchmakingSimpleEvent, bool, bSuccess, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCOMatchmakingSearchCompletedEvent, bool, bSuccess, const TArray<FCOMatchmakingSearchResult>&, Results);

UCLASS()
class COVERTOPS1_API UCOMatchmakingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Matchmaking")
	void HostOnlineSession(FName SessionName, int32 MaxPlayers, bool bIsLAN, ECovertOpsMatchMode Mode);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Matchmaking")
	void FindOnlineSessions(int32 MaxResults, bool bIsLAN);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Matchmaking")
	void JoinOnlineSessionById(const FString& SessionId);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Matchmaking")
	void DestroyOnlineSession(FName SessionName);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Matchmaking")
	void DestroyPrimaryGameSession();

	UFUNCTION(BlueprintPure, Category="Multiplayer|Matchmaking")
	const TArray<FCOMatchmakingSearchResult>& GetCachedSearchResults() const { return LastSearchResults; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Matchmaking")
	bool HasSessionInterface() const;

public:
	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Matchmaking")
	FCOMatchmakingSimpleEvent OnSessionHosted;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Matchmaking")
	FCOMatchmakingSearchCompletedEvent OnSessionsFound;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Matchmaking")
	FCOMatchmakingSimpleEvent OnSessionJoined;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Matchmaking")
	FCOMatchmakingSimpleEvent OnSessionDestroyed;

protected:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TArray<FOnlineSessionSearchResult> LastRawResults;
	TArray<FCOMatchmakingSearchResult> LastSearchResults;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	FString MatchModeToTag(ECovertOpsMatchMode Mode) const;
};
