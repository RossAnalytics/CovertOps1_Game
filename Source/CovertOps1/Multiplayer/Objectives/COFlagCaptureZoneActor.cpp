// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Objectives/COFlagCaptureZoneActor.h"
#include "CovertOps1.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/Modes/COCTFGameMode.h"
#include "Multiplayer/Objectives/COFlagObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ACOFlagCaptureZoneActor::ACOFlagCaptureZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	CaptureSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CaptureSphere"));
	CaptureSphere->SetupAttachment(SceneRoot);
	CaptureSphere->SetSphereRadius(260.0f);
	CaptureSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CaptureSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CaptureSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACOFlagCaptureZoneActor::BeginPlay()
{
	Super::BeginPlay();
	CaptureSphere->OnComponentBeginOverlap.AddDynamic(this, &ACOFlagCaptureZoneActor::OnCaptureSphereBeginOverlap);

	if (!FriendlyFlag || !EnemyFlag)
	{
		TArray<AActor*> FlagActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOFlagObjectiveActor::StaticClass(), FlagActors);
		for (AActor* Actor : FlagActors)
		{
			ACOFlagObjectiveActor* Flag = Cast<ACOFlagObjectiveActor>(Actor);
			if (!Flag)
			{
				continue;
			}

			if (Flag->GetFlagTeamId() == ZoneTeamId && !FriendlyFlag)
			{
				FriendlyFlag = Flag;
			}
			else if (Flag->GetFlagTeamId() != ZoneTeamId && !EnemyFlag)
			{
				EnemyFlag = Flag;
			}
		}
	}
}

void ACOFlagCaptureZoneActor::ConfigureFlags(ACOFlagObjectiveActor* InFriendlyFlag, ACOFlagObjectiveActor* InEnemyFlag)
{
	FriendlyFlag = InFriendlyFlag;
	EnemyFlag = InEnemyFlag;
}

void ACOFlagCaptureZoneActor::OnCaptureSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !FriendlyFlag || !EnemyFlag)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	const ACOMultiplayerPlayerState* PS = Pawn->GetPlayerState<ACOMultiplayerPlayerState>();
	if (!PS || PS->GetTeamId() != ZoneTeamId)
	{
		return;
	}

	if (FriendlyFlag->IsAtHome() && EnemyFlag->GetCarrierPawn() == Pawn)
	{
		EnemyFlag->CaptureBy(Pawn);

		if (ACOCTFGameMode* CTFMode = Cast<ACOCTFGameMode>(GetWorld()->GetAuthGameMode()))
		{
			CTFMode->HandleFlagCaptured(ZoneTeamId);
		}

		UE_LOG(LogCovertOps1, Log, TEXT("Team %d captured enemy flag"), ZoneTeamId);
	}
}
