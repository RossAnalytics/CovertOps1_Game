// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "CovertOpsPerkData.generated.h"

UCLASS(BlueprintType)
class COVERTOPS1_API UCovertOpsPerkData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	FName PerkId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	ECovertOpsPerkTier Tier = ECovertOpsPerkTier::Tier1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	bool bIsProVariant = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Perk")
	int32 ChallengeRequirement = 0;
};
