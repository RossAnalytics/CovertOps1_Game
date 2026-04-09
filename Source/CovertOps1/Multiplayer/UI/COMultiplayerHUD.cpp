// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/UI/COMultiplayerHUD.h"
#include "Multiplayer/Core/COMultiplayerGameState.h"
#include "Multiplayer/UI/COMultiplayerHUDWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACOMultiplayerHUD::ACOMultiplayerHUD()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACOMultiplayerHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass && PlayerOwner)
	{
		HUDWidget = CreateWidget<UCOMultiplayerHUDWidget>(PlayerOwner, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	RefreshFromGameState();
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(HUDRefreshTimer, this, &ACOMultiplayerHUD::RefreshFromGameState, 1.0f, true);
	}
}

void ACOMultiplayerHUD::PushKillfeedEntry(const FString& EntryText)
{
	if (HUDWidget)
	{
		HUDWidget->BP_AddKillfeedEntry(EntryText);
	}
}

void ACOMultiplayerHUD::SetObjectiveStatus(const FString& StatusText)
{
	if (HUDWidget)
	{
		HUDWidget->BP_UpdateObjectiveStatus(StatusText);
	}
}

void ACOMultiplayerHUD::SetObjectivePrompt(const FString& PromptText, bool bVisible)
{
	if (HUDWidget)
	{
		HUDWidget->BP_UpdateObjectivePrompt(PromptText, bVisible);
	}
}

void ACOMultiplayerHUD::SetObjectiveProgress(float NormalizedProgress, bool bVisible, bool bIsDefuse)
{
	if (HUDWidget)
	{
		HUDWidget->BP_UpdateObjectiveProgress(NormalizedProgress, bVisible, bIsDefuse);
	}
}

void ACOMultiplayerHUD::RefreshFromGameState()
{
	if (!HUDWidget || !GetWorld())
	{
		return;
	}

	const ACOMultiplayerGameState* GS = GetWorld()->GetGameState<ACOMultiplayerGameState>();
	if (!GS)
	{
		return;
	}

	HUDWidget->BP_UpdateMatchClock(GS->GetRemainingMatchTime());
	HUDWidget->BP_UpdateTeamScores(GS->GetTeamScore(0), GS->GetTeamScore(1));

	const TArray<int32>& RoundWins = GS->GetRoundWins();
	HUDWidget->BP_UpdateRoundState(
		GS->GetCurrentRound(),
		RoundWins.IsValidIndex(0) ? RoundWins[0] : 0,
		RoundWins.IsValidIndex(1) ? RoundWins[1] : 0);
}
