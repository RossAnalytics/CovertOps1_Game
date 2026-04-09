// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COBackendApiSubsystem.h"
#include "CovertOps1.h"
#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UCOBackendApiSubsystem::RequestMatchmakingTicket(ECovertOpsMatchMode Mode, const FString& Region, int32 PartySize, int32 SkillRating)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("mode"), ModeToBackendString(Mode));
	JsonObject->SetStringField(TEXT("region"), Region);
	JsonObject->SetNumberField(TEXT("partySize"), PartySize);
	JsonObject->SetNumberField(TEXT("skillRating"), SkillRating);

	FString Payload;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	SendJsonRequest(TEXT("/matchmaking/tickets"), TEXT("POST"), Payload,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnMatchmakingTicketResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::SubmitMatchResult(const FString& MatchResultJson)
{
	SendJsonRequest(TEXT("/match/results"), TEXT("POST"), MatchResultJson,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnMatchResultSubmittedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::FetchProfile(const FString& PlayerId)
{
	SendJsonRequest(FString::Printf(TEXT("/profiles/%s"), *PlayerId), TEXT("GET"), FString(),
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnProfileFetchedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::UpdateProfile(const FString& PlayerId, const FString& ProfileJson)
{
	SendJsonRequest(FString::Printf(TEXT("/profiles/%s"), *PlayerId), TEXT("PUT"), ProfileJson,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnProfileUpdatedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::GrantXP(const FString& PlayerId, int32 Amount, const FString& Source)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("playerId"), PlayerId);
	JsonObject->SetNumberField(TEXT("amount"), Amount);
	JsonObject->SetStringField(TEXT("source"), Source);

	FString Payload;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	SendJsonRequest(TEXT("/progression/xp"), TEXT("POST"), Payload,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnXpGrantedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::GrantCredits(const FString& PlayerId, int32 Amount, const FString& Source)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("playerId"), PlayerId);
	JsonObject->SetNumberField(TEXT("amount"), Amount);
	JsonObject->SetStringField(TEXT("source"), Source);

	FString Payload;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	SendJsonRequest(TEXT("/economy/credits/grant"), TEXT("POST"), Payload,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnCreditsGrantedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::ReportPlayer(const FString& ReportedPlayerId, const FString& Reason, const FString& MatchId)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField(TEXT("reportedPlayerId"), ReportedPlayerId);
	JsonObject->SetStringField(TEXT("reason"), Reason);
	if (!MatchId.IsEmpty())
	{
		JsonObject->SetStringField(TEXT("matchId"), MatchId);
	}

	FString Payload;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Payload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	SendJsonRequest(TEXT("/reports/player"), TEXT("POST"), Payload,
		[this](bool bSuccess, int32 StatusCode, const FString& Response)
		{
			OnPlayerReportedResponse.Broadcast(bSuccess, StatusCode, Response);
		});
}

void UCOBackendApiSubsystem::SendJsonRequest(const FString& RelativePath, const FString& Verb, const FString& JsonPayload, FBackendCallback&& Callback) const
{
	const FString Url = BackendBaseUrl + RelativePath;
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	if (!DevBearerToken.IsEmpty())
	{
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *DevBearerToken));
	}

	if (!JsonPayload.IsEmpty() && Verb != TEXT("GET"))
	{
		Request->SetContentAsString(JsonPayload);
	}

	Request->OnProcessRequestComplete().BindLambda(
		[Callback = MoveTemp(Callback), Url](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bConnectedSuccessfully)
		{
			const int32 StatusCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : -1;
			const FString Body = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("No response body");
			const bool bOk = bConnectedSuccessfully && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(StatusCode);

			if (!bOk)
			{
				UE_LOG(LogCovertOps1, Warning, TEXT("Backend request failed. Url=%s Status=%d Body=%s"), *Url, StatusCode, *Body);
			}

			Callback(bOk, StatusCode, Body);
		});

	Request->ProcessRequest();
}

FString UCOBackendApiSubsystem::ModeToBackendString(ECovertOpsMatchMode Mode) const
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
