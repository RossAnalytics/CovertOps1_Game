// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMultiplayerPlayerController.h"
#include "CovertOps1.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/Objectives/COBombObjectiveActor.h"
#include "Multiplayer/Objectives/COBombSiteActor.h"
#include "Multiplayer/Objectives/COFlagObjectiveActor.h"
#include "Multiplayer/UI/COMultiplayerHUD.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACOMultiplayerPlayerController::ACOMultiplayerPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractActionAsset(TEXT("/Game/Variant_Shooter/Input/Actions/IA_Interact.IA_Interact"));
	if (InteractActionAsset.Succeeded())
	{
		InteractAction = InteractActionAsset.Object;
	}
}

void ACOMultiplayerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController() || !InputComponent)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACOMultiplayerPlayerController::StartObjectiveInteractInput);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ACOMultiplayerPlayerController::StopObjectiveInteractInput);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled, this, &ACOMultiplayerPlayerController::StopObjectiveInteractInput);
		}
		else
		{
			UE_LOG(LogCovertOps1, Warning, TEXT("ACOMultiplayerPlayerController: InteractAction is not assigned. Objective interactions are unavailable."));
		}
	}
}

void ACOMultiplayerPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController())
	{
		UpdateObjectivePrompt();
	}
}

void ACOMultiplayerPlayerController::SubmitLoadout(const FLoadoutSpec& InLoadout)
{
	if (HasAuthority())
	{
		if (ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>())
		{
			PS->SetLoadoutSpec(InLoadout);
		}
	}
	else
	{
		ServerSubmitLoadout(InLoadout);
	}
}

void ACOMultiplayerPlayerController::SubmitPerkSelection(const FPerkSelection& InPerks)
{
	if (HasAuthority())
	{
		if (ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>())
		{
			PS->ApplyPerkSelection(InPerks);
		}
	}
	else
	{
		ServerSubmitPerkSelection(InPerks);
	}
}

void ACOMultiplayerPlayerController::TryObjectiveInteract()
{
	bWantsToHoldObjectiveInteract = true;

	if (HasAuthority())
	{
		ServerTryObjectiveInteract_Implementation();
	}
	else
	{
		ServerTryObjectiveInteract();
	}
}

void ACOMultiplayerPlayerController::StopObjectiveInteract()
{
	bWantsToHoldObjectiveInteract = false;

	if (HasAuthority())
	{
		ServerStopObjectiveInteract_Implementation();
	}
	else
	{
		ServerStopObjectiveInteract();
	}
}

void ACOMultiplayerPlayerController::ServerSubmitLoadout_Implementation(const FLoadoutSpec& InLoadout)
{
	if (ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>())
	{
		PS->SetLoadoutSpec(InLoadout);
	}
}

void ACOMultiplayerPlayerController::ServerSubmitPerkSelection_Implementation(const FPerkSelection& InPerks)
{
	if (ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>())
	{
		PS->ApplyPerkSelection(InPerks);
	}
}

void ACOMultiplayerPlayerController::ServerTryObjectiveInteract_Implementation()
{
	APawn* ControlledPawn = GetPawn();
	const ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>();
	if (!ControlledPawn || !PS || !GetWorld())
	{
		return;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const float UseRangeSq = FMath::Square(ObjectiveInteractRange);

	TArray<AActor*> BombActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombObjectiveActor::StaticClass(), BombActors);

	for (AActor* Actor : BombActors)
	{
		ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(Actor);
		if (!Bomb)
		{
			continue;
		}

		if (Bomb->IsInteractionInProgress() && Bomb->GetInteractingPawn() == ControlledPawn)
		{
			return;
		}
	}

	for (AActor* Actor : BombActors)
	{
		ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(Actor);
		if (!Bomb || FVector::DistSquared(PawnLocation, Bomb->GetActorLocation()) > UseRangeSq)
		{
			continue;
		}

		if ((Bomb->GetBombState() == ECOBombState::AtSpawn || Bomb->GetBombState() == ECOBombState::Dropped) &&
			Bomb->PickupBomb(ControlledPawn))
		{
			return;
		}
	}

	TArray<AActor*> SiteActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombSiteActor::StaticClass(), SiteActors);

	for (AActor* Actor : BombActors)
	{
		ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(Actor);
		if (!Bomb)
		{
			continue;
		}

		if (Bomb->GetBombState() == ECOBombState::Carried && Bomb->GetCarrierPawn() == ControlledPawn)
		{
			for (AActor* SiteActor : SiteActors)
			{
				ACOBombSiteActor* Site = Cast<ACOBombSiteActor>(SiteActor);
				if (!Site || Site->GetDefendingTeamId() == PS->GetTeamId())
				{
					continue;
				}

				if (Site->IsPawnInPlantRange(ControlledPawn))
				{
					Bomb->StartPlanting(ControlledPawn, Site);
					return;
				}
			}
		}

		if (Bomb->GetBombState() == ECOBombState::Planted)
		{
			ACOBombSiteActor* Site = Bomb->GetPlantedSite();
			if (Site && Site->GetDefendingTeamId() == PS->GetTeamId() && Site->IsPawnInPlantRange(ControlledPawn))
			{
				Bomb->StartDefuse(ControlledPawn);
				return;
			}
		}
	}

	TArray<AActor*> FlagActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOFlagObjectiveActor::StaticClass(), FlagActors);
	for (AActor* Actor : FlagActors)
	{
		ACOFlagObjectiveActor* Flag = Cast<ACOFlagObjectiveActor>(Actor);
		if (!Flag || FVector::DistSquared(PawnLocation, Flag->GetActorLocation()) > UseRangeSq)
		{
			continue;
		}

		if (Flag->TryPickup(ControlledPawn))
		{
			return;
		}
	}
}

void ACOMultiplayerPlayerController::ServerStopObjectiveInteract_Implementation()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !GetWorld())
	{
		return;
	}

	TArray<AActor*> BombActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombObjectiveActor::StaticClass(), BombActors);
	for (AActor* Actor : BombActors)
	{
		if (ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(Actor))
		{
			Bomb->StopInteractionForPawn(ControlledPawn);
		}
	}
}

void ACOMultiplayerPlayerController::ClientNotifyKillfeedEvent_Implementation(const FString& EventMessage)
{
	UE_LOG(LogCovertOps1, Log, TEXT("[Killfeed] %s"), *EventMessage);

	if (ACOMultiplayerHUD* MultiplayerHUD = Cast<ACOMultiplayerHUD>(GetHUD()))
	{
		MultiplayerHUD->PushKillfeedEntry(EventMessage);
	}
}

void ACOMultiplayerPlayerController::ClientNotifyObjectiveStatus_Implementation(const FString& StatusMessage)
{
	UE_LOG(LogCovertOps1, Log, TEXT("[Objective] %s"), *StatusMessage);

	if (ACOMultiplayerHUD* MultiplayerHUD = Cast<ACOMultiplayerHUD>(GetHUD()))
	{
		MultiplayerHUD->SetObjectiveStatus(StatusMessage);
	}
}

void ACOMultiplayerPlayerController::UpdateObjectivePrompt()
{
	ACOMultiplayerHUD* MultiplayerHUD = Cast<ACOMultiplayerHUD>(GetHUD());
	if (!MultiplayerHUD)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	const ACOMultiplayerPlayerState* PS = GetPlayerState<ACOMultiplayerPlayerState>();
	if (!ControlledPawn || !PS || !GetWorld())
	{
		if (bCachedPromptVisible || !CachedObjectivePrompt.IsEmpty())
		{
			CachedObjectivePrompt.Reset();
			bCachedPromptVisible = false;
			MultiplayerHUD->SetObjectivePrompt(FString(), false);
		}

		MultiplayerHUD->SetObjectiveProgress(0.0f, false, false);
		return;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const float UseRangeSq = FMath::Square(ObjectiveInteractRange);

	bool bPromptVisible = false;
	FString PromptText;
	bool bShowProgress = false;
	bool bIsDefuseProgress = false;
	float ProgressValue = 0.0f;

	TArray<AActor*> BombActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombObjectiveActor::StaticClass(), BombActors);
	for (AActor* Actor : BombActors)
	{
		ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(Actor);
		if (!Bomb)
		{
			continue;
		}

		const bool bIsInteractor = Bomb->IsInteractionInProgress() && (Bomb->GetInteractingPawn() == ControlledPawn);
		if (bIsInteractor)
		{
			bPromptVisible = true;
			bShowProgress = true;
			ProgressValue = Bomb->GetInteractionProgress();
			bIsDefuseProgress = Bomb->GetInteractionType() == ECOBombInteractionType::Defusing;
			PromptText = bIsDefuseProgress ? FString(TEXT("Defusing Bomb...")) : FString(TEXT("Planting Bomb..."));
			break;
		}

		if ((Bomb->GetBombState() == ECOBombState::AtSpawn || Bomb->GetBombState() == ECOBombState::Dropped) &&
			FVector::DistSquared(PawnLocation, Bomb->GetActorLocation()) <= UseRangeSq)
		{
			bPromptVisible = true;
			PromptText = FString(TEXT("Hold Interact: Pick Up Bomb"));
			break;
		}

		if (Bomb->GetBombState() == ECOBombState::Carried && Bomb->GetCarrierPawn() == ControlledPawn)
		{
			TArray<AActor*> SiteActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombSiteActor::StaticClass(), SiteActors);
			for (AActor* SiteActor : SiteActors)
			{
				ACOBombSiteActor* Site = Cast<ACOBombSiteActor>(SiteActor);
				if (!Site || Site->GetDefendingTeamId() == PS->GetTeamId())
				{
					continue;
				}

				if (Site->IsPawnInPlantRange(ControlledPawn))
				{
					bPromptVisible = true;
					PromptText = FString::Printf(TEXT("Hold Interact: Plant Bomb (%s)"), *Site->GetSiteId().ToString());
					break;
				}
			}

			if (bPromptVisible)
			{
				break;
			}
		}

		if (Bomb->GetBombState() == ECOBombState::Planted)
		{
			ACOBombSiteActor* Site = Bomb->GetPlantedSite();
			if (Site && Site->GetDefendingTeamId() == PS->GetTeamId() && Site->IsPawnInPlantRange(ControlledPawn))
			{
				bPromptVisible = true;
				PromptText = FString::Printf(TEXT("Hold Interact: Defuse Bomb (%s)"), *Site->GetSiteId().ToString());
				break;
			}
		}
	}

	if (!bPromptVisible)
	{
		TArray<AActor*> FlagActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOFlagObjectiveActor::StaticClass(), FlagActors);
		for (AActor* Actor : FlagActors)
		{
			ACOFlagObjectiveActor* Flag = Cast<ACOFlagObjectiveActor>(Actor);
			if (!Flag || FVector::DistSquared(PawnLocation, Flag->GetActorLocation()) > UseRangeSq)
			{
				continue;
			}

			if (!Flag->IsCarried() && Flag->GetFlagTeamId() != PS->GetTeamId())
			{
				bPromptVisible = true;
				PromptText = FString(TEXT("Press Interact: Pick Up Flag"));
				break;
			}
		}
	}

	if (bPromptVisible != bCachedPromptVisible || PromptText != CachedObjectivePrompt)
	{
		bCachedPromptVisible = bPromptVisible;
		CachedObjectivePrompt = PromptText;
		MultiplayerHUD->SetObjectivePrompt(PromptText, bPromptVisible);
	}

	MultiplayerHUD->SetObjectiveProgress(ProgressValue, bShowProgress, bIsDefuseProgress);
}

void ACOMultiplayerPlayerController::StartObjectiveInteractInput()
{
	TryObjectiveInteract();
}

void ACOMultiplayerPlayerController::StopObjectiveInteractInput()
{
	StopObjectiveInteract();
}
