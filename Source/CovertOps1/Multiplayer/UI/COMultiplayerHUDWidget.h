// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "COMultiplayerHUDWidget.generated.h"

UCLASS(Abstract)
class COVERTOPS1_API UCOMultiplayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateMatchClock(int32 RemainingSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateTeamScores(int32 Team0Score, int32 Team1Score);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateRoundState(int32 CurrentRound, int32 Team0Rounds, int32 Team1Rounds);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_AddKillfeedEntry(const FString& EntryText);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateObjectiveStatus(const FString& StatusText);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateObjectivePrompt(const FString& PromptText, bool bVisible);

	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void BP_UpdateObjectiveProgress(float NormalizedProgress, bool bVisible, bool bIsDefuse);
};
