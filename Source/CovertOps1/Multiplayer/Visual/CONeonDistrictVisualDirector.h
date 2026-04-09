// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CONeonDistrictVisualDirector.generated.h"

class UCovertOpsVisualProfileData;
class UMaterialParameterCollection;

USTRUCT(BlueprintType)
struct FCOArtBibleGateResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	bool bReadabilityPassed = true;

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	bool bCohesionPassed = true;

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	bool bPerformancePassed = true;

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	bool bAllPassed = true;

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	TArray<FString> Warnings;
};

UCLASS()
class COVERTOPS1_API ACONeonDistrictVisualDirector : public AActor
{
	GENERATED_BODY()

public:
	ACONeonDistrictVisualDirector();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Visual")
	void ApplyVisualProfile();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Visual")
	FCOArtBibleGateResult RunArtBibleGateAudit();

	UFUNCTION(BlueprintPure, Category="Visual")
	const FCOArtBibleGateResult& GetLastAuditResult() const { return LastAuditResult; }

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Visual")
	TObjectPtr<UCovertOpsVisualProfileData> VisualProfile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	bool bUseNightLighting = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	bool bApplyOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	bool bAuditOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	TObjectPtr<UMaterialParameterCollection> EnvironmentParameterCollection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
	FName WetnessScalarParameter = FName(TEXT("NeonWetness"));

	UPROPERTY(BlueprintReadOnly, Category="Visual")
	FCOArtBibleGateResult LastAuditResult;

protected:
	void AddAuditWarning(FCOArtBibleGateResult& Result, const FString& WarningText, bool bReadabilityIssue, bool bCohesionIssue, bool bPerformanceIssue) const;
};
