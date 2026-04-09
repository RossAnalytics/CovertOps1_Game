// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Multiplayer/Modes/COMultiplayerGameMode.h"
#include "COSnDGameMode.generated.h"

class ACOMultiplayerPlayerState;
class ACOBombObjectiveActor;
class ACOBombSiteActor;

UCLASS()
class COVERTOPS1_API ACOSnDGameMode : public ACOMultiplayerGameMode
{
	GENERATED_BODY()

public:
	ACOSnDGameMode();

	virtual bool CanPlayerRespawn() const override { return false; }

	UFUNCTION(BlueprintCallable, Category="Multiplayer|SnD")
	void CompleteRound(int32 WinningTeam, const FString& Reason);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|SnD")
	void NotifyBombPlanted(int32 PlantingTeam);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|SnD")
	void NotifyBombDefused(int32 DefendingTeam);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|SnD")
	void NotifyBombExploded(int32 AttackingTeam);

protected:
	virtual void BeginPlay() override;
	virtual void OnModeStarted() override;
	virtual void RegisterElimination(ACOMultiplayerPlayerState* KillerPS, ACOMultiplayerPlayerState* VictimPS, int32 VictimTeamId, bool bObjectiveKill) override;
	virtual void TickMatchClock() override;

	void StartRound();
	void EvaluateMatchPoint();
	void DiscoverBombObjectives();

	UFUNCTION()
	void OnBombPlantedInternal(ACOBombSiteActor* Site, APawn* Planter);

	UFUNCTION()
	void OnBombDefusedInternal(APawn* Defuser);

	UFUNCTION()
	void OnBombExplodedInternal();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|SnD")
	int32 RoundsToWin = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|SnD")
	int32 MaxRounds = 11;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Multiplayer|SnD")
	float RoundTransitionDelay = 6.0f;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|SnD")
	int32 CurrentAttackingTeam = 0;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|SnD")
	bool bBombPlanted = false;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|SnD")
	TObjectPtr<ACOBombObjectiveActor> BombActor;

	UPROPERTY(BlueprintReadOnly, Category="Multiplayer|SnD")
	TArray<TObjectPtr<ACOBombSiteActor>> BombSites;
};
