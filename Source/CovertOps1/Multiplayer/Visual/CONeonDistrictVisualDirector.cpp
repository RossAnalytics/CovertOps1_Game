// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Visual/CONeonDistrictVisualDirector.h"
#include "CovertOps1.h"
#include "Multiplayer/Data/CovertOpsVisualProfileData.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

ACONeonDistrictVisualDirector::ACONeonDistrictVisualDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACONeonDistrictVisualDirector::BeginPlay()
{
	Super::BeginPlay();

	if (bApplyOnBeginPlay)
	{
		ApplyVisualProfile();
	}

	if (bAuditOnBeginPlay)
	{
		LastAuditResult = RunArtBibleGateAudit();
	}
}

void ACONeonDistrictVisualDirector::ApplyVisualProfile()
{
	if (!GetWorld() || !VisualProfile)
	{
		return;
	}

	const FVector4 SaturationVector(
		VisualProfile->ColorGrading.Saturation,
		VisualProfile->ColorGrading.Saturation,
		VisualProfile->ColorGrading.Saturation,
		1.0f);

	const FVector4 ContrastVector(
		VisualProfile->ColorGrading.Contrast,
		VisualProfile->ColorGrading.Contrast,
		VisualProfile->ColorGrading.Contrast,
		1.0f);

	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		APostProcessVolume* Volume = *It;
		if (!Volume)
		{
			continue;
		}

		Volume->Settings.bOverride_ColorSaturation = true;
		Volume->Settings.ColorSaturation = SaturationVector;
		Volume->Settings.bOverride_ColorContrast = true;
		Volume->Settings.ColorContrast = ContrastVector;
		Volume->Settings.bOverride_FilmShadowTint = true;
		Volume->Settings.FilmShadowTint = VisualProfile->ColorGrading.ShadowTint;
		Volume->Settings.bOverride_FilmWhitePoint = true;
		Volume->Settings.FilmWhitePoint = VisualProfile->ColorGrading.HighlightTint;
		Volume->Settings.bOverride_BloomIntensity = true;
		Volume->Settings.BloomIntensity = VisualProfile->MaxBloom;
	}

	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		AExponentialHeightFog* Fog = *It;
		if (!Fog || !Fog->GetComponent())
		{
			continue;
		}

		Fog->GetComponent()->SetFogDensity(VisualProfile->Weather.FogDensity);
		Fog->GetComponent()->SetFogHeightFalloff(VisualProfile->Weather.FogHeightFalloff);
	}

	const float TargetLux = bUseNightLighting ? VisualProfile->TargetLuxNight : VisualProfile->TargetLuxDay;
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		ADirectionalLight* Light = *It;
		if (!Light || !Light->GetLightComponent())
		{
			continue;
		}

		Light->GetLightComponent()->SetIntensity(TargetLux);
	}

	if (EnvironmentParameterCollection)
	{
		if (UMaterialParameterCollectionInstance* ParameterCollection = GetWorld()->GetParameterCollectionInstance(EnvironmentParameterCollection))
		{
			ParameterCollection->SetScalarParameterValue(WetnessScalarParameter, VisualProfile->Weather.Wetness);
		}
	}
}

FCOArtBibleGateResult ACONeonDistrictVisualDirector::RunArtBibleGateAudit()
{
	FCOArtBibleGateResult Result;
	Result.bReadabilityPassed = true;
	Result.bCohesionPassed = true;
	Result.bPerformancePassed = true;

	if (!VisualProfile)
	{
		AddAuditWarning(Result, FString(TEXT("Visual profile is missing.")), true, true, false);
		Result.bAllPassed = false;
		LastAuditResult = Result;
		return Result;
	}

	if (VisualProfile->AccentPalette.Num() < 3)
	{
		AddAuditWarning(Result, FString(TEXT("Accent palette should include at least 3 colors for lane readability and map cohesion.")), false, true, false);
	}

	if (VisualProfile->ColorGrading.Contrast < 1.0f || VisualProfile->ColorGrading.Contrast > 1.35f)
	{
		AddAuditWarning(Result, FString(TEXT("Contrast is outside readability target range [1.00, 1.35].")), true, false, false);
	}

	if (VisualProfile->ColorGrading.Saturation < 0.95f || VisualProfile->ColorGrading.Saturation > 1.3f)
	{
		AddAuditWarning(Result, FString(TEXT("Saturation is outside cohesion target range [0.95, 1.30].")), false, true, false);
	}

	if (VisualProfile->MaxBloom < 0.05f || VisualProfile->MaxBloom > 0.45f)
	{
		AddAuditWarning(Result, FString(TEXT("Bloom must stay in [0.05, 0.45] to preserve target silhouettes.")), true, false, false);
	}

	if (VisualProfile->Weather.FogDensity > 0.05f)
	{
		AddAuditWarning(Result, FString(TEXT("FogDensity is above the tactical readability ceiling (0.05).")), true, false, true);
	}

	if (VisualProfile->Weather.RainIntensity > 0.0f && VisualProfile->Weather.Wetness < 0.35f)
	{
		AddAuditWarning(Result, FString(TEXT("Rain is enabled but wetness is too low (< 0.35) for cohesive surface response.")), false, true, false);
	}

	if (VisualProfile->TargetLuxNight < 1.0f || VisualProfile->TargetLuxNight > 8.0f)
	{
		AddAuditWarning(Result, FString(TEXT("TargetLuxNight should stay within [1, 8] for readable Neon District night combat.")), true, false, false);
	}

	Result.bAllPassed = Result.bReadabilityPassed && Result.bCohesionPassed && Result.bPerformancePassed;

	for (const FString& Warning : Result.Warnings)
	{
		UE_LOG(LogCovertOps1, Warning, TEXT("[NeonArtGate] %s"), *Warning);
	}

	if (Result.bAllPassed)
	{
		UE_LOG(LogCovertOps1, Log, TEXT("[NeonArtGate] All art bible gates passed for profile %s"), *VisualProfile->GetName());
	}

	LastAuditResult = Result;
	return Result;
}

void ACONeonDistrictVisualDirector::AddAuditWarning(FCOArtBibleGateResult& Result, const FString& WarningText, bool bReadabilityIssue, bool bCohesionIssue, bool bPerformanceIssue) const
{
	Result.Warnings.Add(WarningText);

	if (bReadabilityIssue)
	{
		Result.bReadabilityPassed = false;
	}

	if (bCohesionIssue)
	{
		Result.bCohesionPassed = false;
	}

	if (bPerformanceIssue)
	{
		Result.bPerformancePassed = false;
	}
}
