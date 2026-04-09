// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpawnPolicy.generated.h"

UINTERFACE(BlueprintType)
class COVERTOPS1_API USpawnPolicy : public UInterface
{
	GENERATED_BODY()
};

class COVERTOPS1_API ISpawnPolicy
{
	GENERATED_BODY()

public:
	virtual FTransform ChooseSpawnTransform(AController* RequestingController) const = 0;
};
