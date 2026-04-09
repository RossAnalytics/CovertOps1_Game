// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMultiplayerGameState.generated.h"

UCLASS()
class COVERTOPS1_API ACOMultiplayerGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACOMultiplayerGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetMatchMode(ECovertOpsMatchMode InMatchMode);
	void SetMatchPhase(ECovertOpsMatchPhase InMatchPhase);
	void SetRemainingMatchTime(int32 InSeconds);
	void SetCurrentRound(int32 InRound);
	void SetRoundWins(const TArray<int32>& InRoundWins);
	void AddTeamScore(int32 TeamId, int32 Amount);

	UFUNCTION(BlueprintPure, Category="Multiplayer|Match")
	ECovertOpsMatchMode GetMatchMode() const { return MatchMode; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Match")
	ECovertOpsMatchPhase GetMatchPhase() const { return MatchPhase; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Match")
	int32 GetRemainingMatchTime() const { return RemainingMatchTimeSeconds; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Score")
	int32 GetTeamScore(int32 TeamId) const;

	UFUNCTION(BlueprintPure, Category="Multiplayer|Rounds")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Rounds")
	const TArray<int32>& GetRoundWins() const { return RoundWins; }

protected:
	UPROPERTY(ReplicatedUsing=OnRep_MatchData, BlueprintReadOnly, Category="Multiplayer|Match")
	ECovertOpsMatchMode MatchMode = ECovertOpsMatchMode::TDM;

	UPROPERTY(ReplicatedUsing=OnRep_MatchData, BlueprintReadOnly, Category="Multiplayer|Match")
	ECovertOpsMatchPhase MatchPhase = ECovertOpsMatchPhase::Warmup;

	UPROPERTY(ReplicatedUsing=OnRep_MatchData, BlueprintReadOnly, Category="Multiplayer|Match")
	int32 RemainingMatchTimeSeconds = 0;

	UPROPERTY(ReplicatedUsing=OnRep_ScoreData, BlueprintReadOnly, Category="Multiplayer|Score")
	TArray<int32> TeamScores;

	UPROPERTY(ReplicatedUsing=OnRep_RoundData, BlueprintReadOnly, Category="Multiplayer|Rounds")
	int32 CurrentRound = 1;

	UPROPERTY(ReplicatedUsing=OnRep_RoundData, BlueprintReadOnly, Category="Multiplayer|Rounds")
	TArray<int32> RoundWins;

protected:
	UFUNCTION()
	void OnRep_MatchData();

	UFUNCTION()
	void OnRep_ScoreData();

	UFUNCTION()
	void OnRep_RoundData();
};
