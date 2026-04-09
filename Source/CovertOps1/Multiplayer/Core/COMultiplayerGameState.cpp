// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMultiplayerGameState.h"
#include "Net/UnrealNetwork.h"

ACOMultiplayerGameState::ACOMultiplayerGameState()
{
	bReplicates = true;
	TeamScores.Init(0, 2);
	RoundWins.Init(0, 2);
}

void ACOMultiplayerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACOMultiplayerGameState, MatchMode);
	DOREPLIFETIME(ACOMultiplayerGameState, MatchPhase);
	DOREPLIFETIME(ACOMultiplayerGameState, RemainingMatchTimeSeconds);
	DOREPLIFETIME(ACOMultiplayerGameState, TeamScores);
	DOREPLIFETIME(ACOMultiplayerGameState, CurrentRound);
	DOREPLIFETIME(ACOMultiplayerGameState, RoundWins);
}

void ACOMultiplayerGameState::SetMatchMode(ECovertOpsMatchMode InMatchMode)
{
	MatchMode = InMatchMode;
	OnRep_MatchData();
}

void ACOMultiplayerGameState::SetMatchPhase(ECovertOpsMatchPhase InMatchPhase)
{
	MatchPhase = InMatchPhase;
	OnRep_MatchData();
}

void ACOMultiplayerGameState::SetRemainingMatchTime(int32 InSeconds)
{
	RemainingMatchTimeSeconds = InSeconds;
	OnRep_MatchData();
}

void ACOMultiplayerGameState::SetCurrentRound(int32 InRound)
{
	CurrentRound = FMath::Max(1, InRound);
	OnRep_RoundData();
}

void ACOMultiplayerGameState::SetRoundWins(const TArray<int32>& InRoundWins)
{
	RoundWins = InRoundWins;
	OnRep_RoundData();
}

void ACOMultiplayerGameState::AddTeamScore(int32 TeamId, int32 Amount)
{
	if (TeamScores.Num() <= TeamId || TeamId < 0)
	{
		return;
	}

	TeamScores[TeamId] += Amount;
	OnRep_ScoreData();
}

int32 ACOMultiplayerGameState::GetTeamScore(int32 TeamId) const
{
	if (TeamScores.Num() <= TeamId || TeamId < 0)
	{
		return 0;
	}

	return TeamScores[TeamId];
}

void ACOMultiplayerGameState::OnRep_MatchData()
{
}

void ACOMultiplayerGameState::OnRep_ScoreData()
{
}

void ACOMultiplayerGameState::OnRep_RoundData()
{
}
