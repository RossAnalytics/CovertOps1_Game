// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMultiplayerPlayerController.generated.h"

class UInputAction;

UCLASS()
class COVERTOPS1_API ACOMultiplayerPlayerController : public AShooterPlayerController
{
	GENERATED_BODY()

public:
	ACOMultiplayerPlayerController();

	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Loadout")
	void SubmitLoadout(const FLoadoutSpec& InLoadout);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Perks")
	void SubmitPerkSelection(const FPerkSelection& InPerks);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Objectives")
	void TryObjectiveInteract();

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Objectives")
	void StopObjectiveInteract();

	UFUNCTION(Client, Reliable)
	void ClientNotifyKillfeedEvent(const FString& EventMessage);

	UFUNCTION(Client, Reliable)
	void ClientNotifyObjectiveStatus(const FString& StatusMessage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Objectives")
	float ObjectiveInteractRange = 250.0f;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|Objectives")
	bool bWantsToHoldObjectiveInteract = false;

	UPROPERTY()
	FString CachedObjectivePrompt;

	UPROPERTY()
	bool bCachedPromptVisible = false;

protected:
	UFUNCTION(Server, Reliable)
	void ServerSubmitLoadout(const FLoadoutSpec& InLoadout);

	UFUNCTION(Server, Reliable)
	void ServerSubmitPerkSelection(const FPerkSelection& InPerks);

	UFUNCTION(Server, Reliable)
	void ServerTryObjectiveInteract();

	UFUNCTION(Server, Reliable)
	void ServerStopObjectiveInteract();

	void UpdateObjectivePrompt();
	void StartObjectiveInteractInput();
	void StopObjectiveInteractInput();
};
