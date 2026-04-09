// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "MatchModeInterface.generated.h"

UINTERFACE(BlueprintType)
class COVERTOPS1_API UMatchModeInterface : public UInterface
{
	GENERATED_BODY()
};

class COVERTOPS1_API IMatchModeInterface
{
	GENERATED_BODY()

public:
	virtual ECovertOpsMatchMode GetMatchMode() const = 0;
	virtual bool IsRoundBasedMode() const = 0;
	virtual bool CanPlayerRespawn() const = 0;
};
