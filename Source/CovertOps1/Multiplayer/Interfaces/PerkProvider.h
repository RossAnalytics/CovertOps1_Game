// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "PerkProvider.generated.h"

UINTERFACE(BlueprintType)
class COVERTOPS1_API UPerkProvider : public UInterface
{
	GENERATED_BODY()
};

class COVERTOPS1_API IPerkProvider
{
	GENERATED_BODY()

public:
	virtual void ApplyPerkSelection(const FPerkSelection& InSelection) = 0;
	virtual bool HasPerk(FName PerkId) const = 0;
};
