// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CovertOpsScorestreakData.generated.h"

UCLASS(BlueprintType)
class COVERTOPS1_API UCovertOpsScorestreakData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	FName StreakId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	int32 RequiredScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	float DurationSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scorestreak")
	float HealthPool = 0.0f;
};
