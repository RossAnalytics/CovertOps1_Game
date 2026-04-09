// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerTypes.generated.h"

UENUM(BlueprintType)
enum class ECovertOpsMatchMode : uint8
{
	TDM UMETA(DisplayName = "Team Deathmatch"),
	CTF UMETA(DisplayName = "Capture The Flag"),
	SnD UMETA(DisplayName = "Search and Destroy")
};

UENUM(BlueprintType)
enum class ECovertOpsMatchPhase : uint8
{
	Warmup UMETA(DisplayName = "Warmup"),
	InProgress UMETA(DisplayName = "In Progress"),
	RoundEnd UMETA(DisplayName = "Round End"),
	Completed UMETA(DisplayName = "Completed")
};

UENUM(BlueprintType)
enum class ECovertOpsStance : uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouched UMETA(DisplayName = "Crouched"),
	Prone UMETA(DisplayName = "Prone"),
	Sliding UMETA(DisplayName = "Sliding")
};

UENUM(BlueprintType)
enum class ECovertOpsPerkTier : uint8
{
	Tier1 UMETA(DisplayName = "Tier 1"),
	Tier2 UMETA(DisplayName = "Tier 2"),
	Tier3 UMETA(DisplayName = "Tier 3")
};

USTRUCT(BlueprintType)
struct COVERTOPS1_API FLoadoutSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PrimaryWeaponId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SecondaryWeaponId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LethalEquipmentId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TacticalEquipmentId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> PrimaryAttachments;
};

USTRUCT(BlueprintType)
struct COVERTOPS1_API FPerkSelection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tier1Perk = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tier2Perk = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Tier3Perk = NAME_None;
};

USTRUCT(BlueprintType)
struct COVERTOPS1_API FScorestreakState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StreakId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScoreRequired = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReadyToUse = false;
};

USTRUCT(BlueprintType)
struct COVERTOPS1_API FMatchResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECovertOpsMatchMode MatchMode = ECovertOpsMatchMode::TDM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WinningTeam = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> TeamScores;
};
