// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Objectives/COBombSiteActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ACOBombSiteActor::ACOBombSiteActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	SiteMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SiteMesh"));
	SiteMesh->SetupAttachment(SceneRoot);
	SiteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SiteSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SiteSphere"));
	SiteSphere->SetupAttachment(SceneRoot);
	SiteSphere->SetSphereRadius(260.0f);
	SiteSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SiteSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	SiteSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

bool ACOBombSiteActor::IsPawnInPlantRange(const APawn* Pawn) const
{
	if (!Pawn || !SiteSphere)
	{
		return false;
	}

	return FVector::DistSquared(Pawn->GetActorLocation(), SiteSphere->GetComponentLocation()) <= FMath::Square(SiteSphere->GetScaledSphereRadius());
}
