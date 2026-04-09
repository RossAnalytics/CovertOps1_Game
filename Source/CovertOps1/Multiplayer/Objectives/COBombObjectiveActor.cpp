// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Objectives/COBombObjectiveActor.h"
#include "CovertOps1.h"
#include "Multiplayer/Core/COMultiplayerCharacter.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/Objectives/COBombSiteActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ACOBombObjectiveActor::ACOBombObjectiveActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(SceneRoot);
	BombMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BombInteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BombInteractionSphere"));
	BombInteractionSphere->SetupAttachment(SceneRoot);
	BombInteractionSphere->SetSphereRadius(180.0f);
	BombInteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BombInteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	BombInteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACOBombObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	SpawnTransform = GetActorTransform();
}

void ACOBombObjectiveActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || InteractionType == ECOBombInteractionType::None)
	{
		return;
	}

	if (!ValidateInteraction(ActiveInteractor.Get(), ActiveInteractionSite.Get(), InteractionType))
	{
		ClearInteraction(InteractionType);
		return;
	}

	if (ActiveInteractionDuration <= KINDA_SMALL_NUMBER)
	{
		ClearInteraction(InteractionType);
		return;
	}

	InteractionElapsedSeconds = FMath::Min(InteractionElapsedSeconds + DeltaSeconds, ActiveInteractionDuration);
	InteractionProgress = FMath::Clamp(InteractionElapsedSeconds / ActiveInteractionDuration, 0.0f, 1.0f);
	OnBombInteractionProgress.Broadcast(InteractionType, ActiveInteractor.Get(), InteractionProgress);

	if (InteractionProgress < 1.0f)
	{
		return;
	}

	const ECOBombInteractionType CompletedType = InteractionType;
	if (CompletedType == ECOBombInteractionType::Planting)
	{
		if (!PlantBomb(ActiveInteractionSite.Get(), ActiveInteractor.Get()))
		{
			ClearInteraction(CompletedType);
		}
	}
	else if (CompletedType == ECOBombInteractionType::Defusing)
	{
		CompleteDefuse();
	}
}

void ACOBombObjectiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACOBombObjectiveActor, BombState);
	DOREPLIFETIME(ACOBombObjectiveActor, CarrierPawn);
	DOREPLIFETIME(ACOBombObjectiveActor, PlantedSite);
	DOREPLIFETIME(ACOBombObjectiveActor, InteractionType);
	DOREPLIFETIME(ACOBombObjectiveActor, InteractionProgress);
	DOREPLIFETIME(ACOBombObjectiveActor, ActiveInteractor);
	DOREPLIFETIME(ACOBombObjectiveActor, ActiveInteractionSite);
}

bool ACOBombObjectiveActor::PickupBomb(APawn* NewCarrier)
{
	if (!HasAuthority() || !IsValid(NewCarrier))
	{
		return false;
	}

	if (BombState != ECOBombState::AtSpawn && BombState != ECOBombState::Dropped)
	{
		return false;
	}

	ClearInteraction(ECOBombInteractionType::None);
	GetWorldTimerManager().ClearTimer(FuseTimer);
	GetWorldTimerManager().ClearTimer(DefuseTimer);

	CarrierPawn = NewCarrier;
	PlantedSite = nullptr;
	BombState = ECOBombState::Carried;

	RefreshAttachment();
	OnBombPickedUp.Broadcast(NewCarrier);
	OnRep_BombState();
	return true;
}

bool ACOBombObjectiveActor::PlantBomb(ACOBombSiteActor* Site, APawn* Planter)
{
	if (!HasAuthority() || !ValidateInteraction(Planter, Site, ECOBombInteractionType::Planting))
	{
		return false;
	}

	GetWorldTimerManager().ClearTimer(FuseTimer);
	GetWorldTimerManager().ClearTimer(DefuseTimer);

	CarrierPawn = nullptr;
	PlantedSite = Site;
	BombState = ECOBombState::Planted;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(Site->GetActorLocation());
	SetActorRotation(Site->GetActorRotation());

	ClearInteraction(ECOBombInteractionType::None);
	GetWorldTimerManager().SetTimer(FuseTimer, this, &ACOBombObjectiveActor::OnFuseExpired, FuseDurationSeconds, false);

	OnBombPlanted.Broadcast(Site, Planter);
	OnRep_BombState();
	return true;
}

bool ACOBombObjectiveActor::StartPlanting(APawn* Planter, ACOBombSiteActor* Site)
{
	if (!HasAuthority() || !ValidateInteraction(Planter, Site, ECOBombInteractionType::Planting))
	{
		return false;
	}

	StartInteraction(ECOBombInteractionType::Planting, Planter, Site, PlantDurationSeconds);
	return true;
}

bool ACOBombObjectiveActor::StartDefuse(APawn* Defuser)
{
	if (!HasAuthority() || !PlantedSite)
	{
		return false;
	}

	if (!ValidateInteraction(Defuser, PlantedSite, ECOBombInteractionType::Defusing))
	{
		return false;
	}

	StartInteraction(ECOBombInteractionType::Defusing, Defuser, PlantedSite, DefuseDurationSeconds);
	return true;
}

void ACOBombObjectiveActor::CancelDefuse()
{
	if (!HasAuthority())
	{
		return;
	}

	if (InteractionType == ECOBombInteractionType::Defusing)
	{
		ClearInteraction(ECOBombInteractionType::Defusing);
	}
}

void ACOBombObjectiveActor::StopInteractionForPawn(APawn* Pawn)
{
	if (!HasAuthority() || !Pawn || ActiveInteractor != Pawn)
	{
		return;
	}

	if (InteractionType != ECOBombInteractionType::None)
	{
		ClearInteraction(InteractionType);
	}
}

void ACOBombObjectiveActor::DropBomb(const FVector& DropLocation)
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(FuseTimer);
	GetWorldTimerManager().ClearTimer(DefuseTimer);
	ClearInteraction(ECOBombInteractionType::None);

	CarrierPawn = nullptr;
	PlantedSite = nullptr;
	BombState = ECOBombState::Dropped;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(DropLocation);

	OnRep_BombState();
}

void ACOBombObjectiveActor::ResetBombToSpawn()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(FuseTimer);
	GetWorldTimerManager().ClearTimer(DefuseTimer);
	ClearInteraction(ECOBombInteractionType::None);

	CarrierPawn = nullptr;
	PlantedSite = nullptr;
	BombState = ECOBombState::AtSpawn;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorTransform(SpawnTransform);

	OnRep_BombState();
}

void ACOBombObjectiveActor::OnRep_BombState()
{
	RefreshAttachment();
	BP_OnBombStateChanged(BombState, CarrierPawn.Get(), PlantedSite.Get());
}

void ACOBombObjectiveActor::OnRep_CarrierPawn()
{
	RefreshAttachment();
	BP_OnBombStateChanged(BombState, CarrierPawn.Get(), PlantedSite.Get());
}

void ACOBombObjectiveActor::OnRep_PlantedSite()
{
	BP_OnBombStateChanged(BombState, CarrierPawn.Get(), PlantedSite.Get());
}

void ACOBombObjectiveActor::OnRep_InteractionState()
{
	BP_OnBombInteractionChanged(InteractionType, InteractionProgress, ActiveInteractor.Get());
}

void ACOBombObjectiveActor::RefreshAttachment()
{
	if (BombState == ECOBombState::Carried && CarrierPawn)
	{
		AttachToActor(CarrierPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(FVector(0.0f, 18.0f, 58.0f));
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void ACOBombObjectiveActor::CompleteDefuse()
{
	if (!HasAuthority() || BombState != ECOBombState::Planted)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(FuseTimer);
	GetWorldTimerManager().ClearTimer(DefuseTimer);

	APawn* Defuser = ActiveInteractor.Get();
	BombState = ECOBombState::Defused;
	ClearInteraction(ECOBombInteractionType::None);
	OnBombDefused.Broadcast(Defuser);
	OnRep_BombState();
}

void ACOBombObjectiveActor::OnFuseExpired()
{
	if (!HasAuthority() || BombState != ECOBombState::Planted)
	{
		return;
	}

	BombState = ECOBombState::Exploded;
	ClearInteraction(ECOBombInteractionType::None);
	OnBombExploded.Broadcast();
	OnRep_BombState();
}

bool ACOBombObjectiveActor::ValidateInteraction(APawn* Pawn, ACOBombSiteActor* Site, ECOBombInteractionType InType) const
{
	if (!HasAuthority() || !Pawn || !Site || InType == ECOBombInteractionType::None)
	{
		return false;
	}

	const ACOMultiplayerCharacter* Character = Cast<ACOMultiplayerCharacter>(Pawn);
	if (!Character || Character->IsDead())
	{
		return false;
	}

	if (Character->GetVelocity().SizeSquared() > FMath::Square(MovementCancelSpeed))
	{
		return false;
	}

	if (Character->WasDamagedRecently(DamageCancelWindowSeconds))
	{
		return false;
	}

	const ACOMultiplayerPlayerState* PlayerState = Pawn->GetPlayerState<ACOMultiplayerPlayerState>();
	if (!PlayerState)
	{
		return false;
	}

	if (!Site->IsPawnInPlantRange(Pawn))
	{
		return false;
	}

	if (InType == ECOBombInteractionType::Planting)
	{
		return BombState == ECOBombState::Carried &&
			CarrierPawn == Pawn &&
			PlayerState->GetTeamId() != Site->GetDefendingTeamId();
	}

	if (InType == ECOBombInteractionType::Defusing)
	{
		return BombState == ECOBombState::Planted &&
			PlantedSite == Site &&
			PlayerState->GetTeamId() == Site->GetDefendingTeamId();
	}

	return false;
}

void ACOBombObjectiveActor::ClearInteraction(ECOBombInteractionType CancelledType)
{
	const ECOBombInteractionType PreviousType = InteractionType;
	APawn* PreviousInteractor = ActiveInteractor.Get();

	InteractionType = ECOBombInteractionType::None;
	InteractionProgress = 0.0f;
	ActiveInteractor = nullptr;
	ActiveInteractionSite = nullptr;
	InteractionElapsedSeconds = 0.0f;
	ActiveInteractionDuration = 0.0f;
	OnRep_InteractionState();

	if (CancelledType != ECOBombInteractionType::None && PreviousType != ECOBombInteractionType::None)
	{
		OnBombInteractionCancelled.Broadcast(CancelledType, PreviousInteractor);
	}
}

void ACOBombObjectiveActor::StartInteraction(ECOBombInteractionType InType, APawn* Interactor, ACOBombSiteActor* Site, float DurationSeconds)
{
	if (!HasAuthority() || InType == ECOBombInteractionType::None || !Interactor || !Site || DurationSeconds <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (ActiveInteractor && ActiveInteractor != Interactor && InteractionType != ECOBombInteractionType::None)
	{
		ClearInteraction(InteractionType);
	}

	InteractionType = InType;
	ActiveInteractor = Interactor;
	ActiveInteractionSite = Site;
	InteractionElapsedSeconds = 0.0f;
	ActiveInteractionDuration = DurationSeconds;
	InteractionProgress = 0.0f;
	OnBombInteractionStarted.Broadcast(InType, Interactor);
	OnBombInteractionProgress.Broadcast(InType, Interactor, InteractionProgress);
	OnRep_InteractionState();
}
