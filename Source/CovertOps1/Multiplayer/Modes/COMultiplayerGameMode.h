// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Multiplayer/Interfaces/MatchModeInterface.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMultiplayerGameMode.generated.h"

class UCovertOpsSpawnDirectorComponent;
class ACOMultiplayerPlayerState;

UCLASS()
class COVERTOPS1_API ACOMultiplayerGameMode : public AGameModeBase, public IMatchModeInterface
{
	GENERATED_BODY()

public:
	ACOMultiplayerGameMode();

	virtual void BeginPlay() override;
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual ECovertOpsMatchMode GetMatchMode() const override { return MatchMode; }
	virtual bool IsRoundBasedMode() const override { return bRoundBased; }
	virtual bool CanPlayerRespawn() const override { return bAllowRespawns; }

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Score")
	virtual void RegisterElimination(ACOMultiplayerPlayerState* KillerPS, ACOMultiplayerPlayerState* VictimPS, int32 VictimTeamId, bool bObjectiveKill);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Match")
	virtual void EndMatchWithWinner(int32 WinningTeam, const FString& Reason);

	UFUNCTION(BlueprintPure, Category="Multiplayer|Match")
	int32 GetScoreLimit() const { return ScoreLimit; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Match")
	int32 GetTimeLimitSeconds() const { return TimeLimitSeconds; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Multiplayer")
	TObjectPtr<UCovertOpsSpawnDirectorComponent> SpawnDirector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	ECovertOpsMatchMode MatchMode = ECovertOpsMatchMode::TDM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	bool bRoundBased = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	bool bAllowRespawns = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	int32 ScoreLimit = 75;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	int32 TimeLimitSeconds = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|Mode")
	float RespawnDelaySeconds = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|Mode")
	int32 NextTeamAssignment = 0;

protected:
	FTimerHandle MatchClockTimer;
	double MatchStartTime = 0.0;

protected:
	virtual bool CanControllerRespawn(AController* Controller) const;
	virtual void OnModeStarted();
	virtual void TickMatchClock();
	virtual void ScheduleRespawn(AController* Controller, float DelaySeconds);
	void BroadcastObjectiveStatus(const FString& StatusText) const;
};
