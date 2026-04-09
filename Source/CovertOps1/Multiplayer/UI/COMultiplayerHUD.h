// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "COMultiplayerHUD.generated.h"

class UCOMultiplayerHUDWidget;
class ACOMultiplayerGameState;

UCLASS()
class COVERTOPS1_API ACOMultiplayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACOMultiplayerHUD();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void PushKillfeedEntry(const FString& EntryText);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetObjectiveStatus(const FString& StatusText);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetObjectivePrompt(const FString& PromptText, bool bVisible);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetObjectiveProgress(float NormalizedProgress, bool bVisible, bool bIsDefuse);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
	TSubclassOf<UCOMultiplayerHUDWidget> HUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category="HUD")
	TObjectPtr<UCOMultiplayerHUDWidget> HUDWidget;

	FTimerHandle HUDRefreshTimer;

protected:
	void RefreshFromGameState();
};
