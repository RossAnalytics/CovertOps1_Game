// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Modes/COCTFGameMode.h"
#include "Multiplayer/Core/COMultiplayerGameState.h"

ACOCTFGameMode::ACOCTFGameMode()
{
	MatchMode = ECovertOpsMatchMode::CTF;
	bRoundBased = false;
	bAllowRespawns = true;
	ScoreLimit = 3;
	TimeLimitSeconds = 900;
}

void ACOCTFGameMode::HandleFlagCaptured(int32 CapturingTeam)
{
	if (!HasAuthority())
	{
		return;
	}

	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		GS->AddTeamScore(CapturingTeam, 1);

		if (GS->GetTeamScore(CapturingTeam) >= ScoreLimit)
		{
			EndMatchWithWinner(CapturingTeam, FString(TEXT("Flag capture limit reached")));
		}
	}

	BroadcastObjectiveStatus(FString::Printf(TEXT("Team %d captured the flag"), CapturingTeam));
}
