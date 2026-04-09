// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Objectives/COFlagObjectiveActor.h"
#include "CovertOps1.h"
#include "Multiplayer/Core/COMultiplayerCharacter.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ACOFlagObjectiveActor::ACOFlagObjectiveActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	FlagMesh->SetupAttachment(SceneRoot);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(SceneRoot);
	PickupSphere->SetSphereRadius(150.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACOFlagObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	HomeTransform = GetActorTransform();
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ACOFlagObjectiveActor::OnPickupSphereBeginOverlap);
}

void ACOFlagObjectiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACOFlagObjectiveActor, FlagState);
	DOREPLIFETIME(ACOFlagObjectiveActor, CarrierPawn);
}

bool ACOFlagObjectiveActor::TryPickup(APawn* NewCarrier)
{
	if (!HasAuthority() || !IsValid(NewCarrier) || FlagState == ECOFlagState::Carried)
	{
		return false;
	}

	ACOMultiplayerPlayerState* CarrierPS = NewCarrier->GetPlayerState<ACOMultiplayerPlayerState>();
	if (!CarrierPS || CarrierPS->GetTeamId() == FlagTeamId)
	{
		return false;
	}

	CarrierPawn = NewCarrier;
	FlagState = ECOFlagState::Carried;
	GetWorldTimerManager().ClearTimer(AutoReturnTimer);

	if (ACOMultiplayerCharacter* Character = Cast<ACOMultiplayerCharacter>(NewCarrier))
	{
		SetCarrierCharacterFlag(Character, true);
	}

	RefreshAttachment();
	BP_OnFlagStateChanged(FlagState, CarrierPawn, false);
	return true;
}

void ACOFlagObjectiveActor::DropFlag(const FVector& DropLocation, bool bCaptured)
{
	if (!HasAuthority())
	{
		return;
	}

	if (ACOMultiplayerCharacter* Character = Cast<ACOMultiplayerCharacter>(CarrierPawn))
	{
		SetCarrierCharacterFlag(Character, false);
	}

	CarrierPawn = nullptr;

	if (bCaptured)
	{
		ReturnToBase();
		return;
	}

	FlagState = ECOFlagState::Dropped;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocation(DropLocation);
	GetWorldTimerManager().SetTimer(AutoReturnTimer, this, &ACOFlagObjectiveActor::OnAutoReturnExpired, AutoReturnDelaySeconds, false);
	OnRep_FlagState();
	BP_OnFlagStateChanged(FlagState, CarrierPawn, bCaptured);
}

void ACOFlagObjectiveActor::ReturnToBase()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(AutoReturnTimer);
	CarrierPawn = nullptr;
	FlagState = ECOFlagState::AtHome;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorTransform(HomeTransform);
	OnRep_FlagState();
	BP_OnFlagStateChanged(FlagState, CarrierPawn, false);
}

void ACOFlagObjectiveActor::CaptureBy(APawn* CapturingPawn)
{
	if (!HasAuthority() || CapturingPawn != CarrierPawn)
	{
		return;
	}

	DropFlag(GetActorLocation(), true);
	BP_OnFlagStateChanged(ECOFlagState::AtHome, nullptr, true);
	UE_LOG(LogCovertOps1, Log, TEXT("CTF flag for team %d captured by %s"), FlagTeamId, *GetNameSafe(CapturingPawn));
}

void ACOFlagObjectiveActor::OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || FlagState == ECOFlagState::Carried)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	if (FlagState == ECOFlagState::Dropped)
	{
		const ACOMultiplayerPlayerState* PS = Pawn->GetPlayerState<ACOMultiplayerPlayerState>();
		if (PS && PS->GetTeamId() == FlagTeamId)
		{
			ReturnToBase();
			return;
		}
	}

	TryPickup(Pawn);
}

void ACOFlagObjectiveActor::OnRep_FlagState()
{
	RefreshAttachment();
}

void ACOFlagObjectiveActor::OnRep_CarrierPawn()
{
	RefreshAttachment();
}

void ACOFlagObjectiveActor::RefreshAttachment()
{
	if (FlagState == ECOFlagState::Carried && CarrierPawn)
	{
		AttachToActor(CarrierPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	}
	else
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void ACOFlagObjectiveActor::SetCarrierCharacterFlag(ACOMultiplayerCharacter* Character, bool bOwnsFlag)
{
	if (!Character)
	{
		return;
	}

	Character->SetCarriedFlag(bOwnsFlag ? this : nullptr);
}

void ACOFlagObjectiveActor::OnAutoReturnExpired()
{
	ReturnToBase();
}
