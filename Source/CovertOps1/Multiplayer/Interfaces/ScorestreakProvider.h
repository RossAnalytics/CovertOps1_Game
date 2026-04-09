// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "ScorestreakProvider.generated.h"

UINTERFACE(BlueprintType)
class COVERTOPS1_API UScorestreakProvider : public UInterface
{
	GENERATED_BODY()
};

class COVERTOPS1_API IScorestreakProvider
{
	GENERATED_BODY()

public:
	virtual void AddScorePoints(int32 Amount, const FName& SourceTag) = 0;
	virtual bool ConsumeScorestreak(FName StreakId) = 0;
	virtual const TArray<FScorestreakState>& GetScorestreakStates() const = 0;
};
