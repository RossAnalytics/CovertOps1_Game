// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Modes/COTDMGameMode.h"

ACOTDMGameMode::ACOTDMGameMode()
{
	MatchMode = ECovertOpsMatchMode::TDM;
	bRoundBased = false;
	bAllowRespawns = true;
	ScoreLimit = 75;
	TimeLimitSeconds = 600;
}
