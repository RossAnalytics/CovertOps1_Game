// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "COBombSiteActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class COVERTOPS1_API ACOBombSiteActor : public AActor
{
	GENERATED_BODY()

public:
	ACOBombSiteActor();

	UFUNCTION(BlueprintPure, Category="SnD")
	bool IsPawnInPlantRange(const APawn* Pawn) const;

	UFUNCTION(BlueprintPure, Category="SnD")
	FName GetSiteId() const { return SiteId; }

	UFUNCTION(BlueprintPure, Category="SnD")
	int32 GetDefendingTeamId() const { return DefendingTeamId; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> SiteMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> SiteSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	FName SiteId = FName(TEXT("A"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	int32 DefendingTeamId = 0;
};
