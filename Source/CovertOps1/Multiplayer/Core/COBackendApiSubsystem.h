// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COBackendApiSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCOBackendResponseEvent, bool, bSuccess, int32, StatusCode, const FString&, Payload);

UCLASS(Config=Game)
class COVERTOPS1_API UCOBackendApiSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Backend")
	FString BackendBaseUrl = TEXT("http://localhost:8080");

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category="Backend")
	FString DevBearerToken = TEXT("dev-player");

public:
	UFUNCTION(BlueprintCallable, Category="Backend")
	void RequestMatchmakingTicket(ECovertOpsMatchMode Mode, const FString& Region, int32 PartySize, int32 SkillRating);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void SubmitMatchResult(const FString& MatchResultJson);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void FetchProfile(const FString& PlayerId);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void UpdateProfile(const FString& PlayerId, const FString& ProfileJson);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void GrantXP(const FString& PlayerId, int32 Amount, const FString& Source);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void GrantCredits(const FString& PlayerId, int32 Amount, const FString& Source);

	UFUNCTION(BlueprintCallable, Category="Backend")
	void ReportPlayer(const FString& ReportedPlayerId, const FString& Reason, const FString& MatchId);

public:
	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnMatchmakingTicketResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnMatchResultSubmittedResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnProfileFetchedResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnProfileUpdatedResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnXpGrantedResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnCreditsGrantedResponse;

	UPROPERTY(BlueprintAssignable, Category="Backend")
	FCOBackendResponseEvent OnPlayerReportedResponse;

private:
	using FBackendCallback = TFunction<void(bool, int32, const FString&)>;

	void SendJsonRequest(const FString& RelativePath, const FString& Verb, const FString& JsonPayload, FBackendCallback&& Callback) const;
	FString ModeToBackendString(ECovertOpsMatchMode Mode) const;
};
