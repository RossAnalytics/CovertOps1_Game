// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CovertOpsVisualProfileData.generated.h"

USTRUCT(BlueprintType)
struct FCOColorGradingProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FLinearColor ShadowTint = FLinearColor(0.95f, 0.98f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FLinearColor MidtoneTint = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FLinearColor HighlightTint = FLinearColor(1.0f, 0.99f, 0.95f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float Contrast = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float Saturation = 1.08f;
};

USTRUCT(BlueprintType)
struct FCOVolumetricWeatherProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float FogDensity = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float FogHeightFalloff = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float RainIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float SandstormIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float Wetness = 0.0f;
};

UCLASS(BlueprintType)
class COVERTOPS1_API UCovertOpsVisualProfileData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FName MapId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FText AtmosphereDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FCOColorGradingProfile ColorGrading;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FCOVolumetricWeatherProfile Weather;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	TArray<FLinearColor> AccentPalette;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float TargetLuxDay = 25000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float TargetLuxNight = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	float MaxBloom = 0.35f;
};
