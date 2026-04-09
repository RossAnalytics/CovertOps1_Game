// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Multiplayer/Modes/COMultiplayerGameMode.h"
#include "COCTFGameMode.generated.h"

UCLASS()
class COVERTOPS1_API ACOCTFGameMode : public ACOMultiplayerGameMode
{
	GENERATED_BODY()

public:
	ACOCTFGameMode();

	UFUNCTION(BlueprintCallable, Category="Multiplayer|CTF")
	void HandleFlagCaptured(int32 CapturingTeam);
};
