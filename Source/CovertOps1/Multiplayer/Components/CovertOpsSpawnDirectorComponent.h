// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Multiplayer/Interfaces/SpawnPolicy.h"
#include "CovertOpsSpawnDirectorComponent.generated.h"

class APlayerStart;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COVERTOPS1_API UCovertOpsSpawnDirectorComponent : public UActorComponent, public ISpawnPolicy
{
	GENERATED_BODY()

public:
	UCovertOpsSpawnDirectorComponent();

	virtual FTransform ChooseSpawnTransform(AController* RequestingController) const override;

	APlayerStart* SelectBestPlayerStart(AController* RequestingController) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn")
	float MinEnemyDistance = 1000.0f;
};
