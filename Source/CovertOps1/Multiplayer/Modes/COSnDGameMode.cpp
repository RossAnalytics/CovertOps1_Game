// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Modes/COSnDGameMode.h"
#include "CovertOps1.h"
#include "Multiplayer/Core/COMultiplayerGameState.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/Objectives/COBombObjectiveActor.h"
#include "Multiplayer/Objectives/COBombSiteActor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACOSnDGameMode::ACOSnDGameMode()
{
	MatchMode = ECovertOpsMatchMode::SnD;
	bRoundBased = true;
	bAllowRespawns = false;
	ScoreLimit = 0;
	TimeLimitSeconds = 150;
}

void ACOSnDGameMode::BeginPlay()
{
	Super::BeginPlay();
	DiscoverBombObjectives();
	StartRound();
}

void ACOSnDGameMode::OnModeStarted()
{
	bBombPlanted = false;
	BroadcastObjectiveStatus(FString::Printf(TEXT("Round live: Team %d attacking"), CurrentAttackingTeam));
}

void ACOSnDGameMode::RegisterElimination(ACOMultiplayerPlayerState* KillerPS, ACOMultiplayerPlayerState* VictimPS, int32 VictimTeamId, bool bObjectiveKill)
{
	Super::RegisterElimination(KillerPS, VictimPS, VictimTeamId, bObjectiveKill);

	if (!HasAuthority())
	{
		return;
	}

	int32 AliveTeam0 = 0;
	int32 AliveTeam1 = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const AController* Controller = It->Get();
		if (!Controller || !Controller->GetPawn())
		{
			continue;
		}

		const ACOMultiplayerPlayerState* PS = Controller->GetPlayerState<ACOMultiplayerPlayerState>();
		if (!PS)
		{
			continue;
		}

		if (PS->GetTeamId() == 0)
		{
			++AliveTeam0;
		}
		else if (PS->GetTeamId() == 1)
		{
			++AliveTeam1;
		}
	}

	if (AliveTeam0 == 0)
	{
		if (bBombPlanted && CurrentAttackingTeam == 0)
		{
			BroadcastObjectiveStatus(FString(TEXT("Attackers eliminated. Bomb still active.")));
		}
		else
		{
			CompleteRound(1, FString(TEXT("Elimination")));
		}
	}
	else if (AliveTeam1 == 0)
	{
		if (bBombPlanted && CurrentAttackingTeam == 1)
		{
			BroadcastObjectiveStatus(FString(TEXT("Attackers eliminated. Bomb still active.")));
		}
		else
		{
			CompleteRound(0, FString(TEXT("Elimination")));
		}
	}
}

void ACOSnDGameMode::CompleteRound(int32 WinningTeam, const FString& Reason)
{
	if (!HasAuthority())
	{
		return;
	}

	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		TArray<int32> RoundWins = GS->GetRoundWins();
		if (RoundWins.Num() < 2)
		{
			RoundWins.Init(0, 2);
		}

		if (RoundWins.IsValidIndex(WinningTeam))
		{
			RoundWins[WinningTeam] += 1;
		}

		GS->SetRoundWins(RoundWins);
		GS->SetMatchPhase(ECovertOpsMatchPhase::RoundEnd);
		GS->SetCurrentRound(GS->GetCurrentRound() + 1);

		UE_LOG(LogCovertOps1, Log, TEXT("SnD round complete. Team %d won by %s"), WinningTeam, *Reason);
	}

	BroadcastObjectiveStatus(FString::Printf(TEXT("Team %d won round (%s)"), WinningTeam, *Reason));

	EvaluateMatchPoint();
}

void ACOSnDGameMode::NotifyBombPlanted(int32 PlantingTeam)
{
	if (!HasAuthority())
	{
		return;
	}

	bBombPlanted = true;
	BroadcastObjectiveStatus(FString::Printf(TEXT("Bomb planted by team %d"), PlantingTeam));

	UE_LOG(LogCovertOps1, Log, TEXT("Bomb planted by team %d"), PlantingTeam);
}

void ACOSnDGameMode::NotifyBombDefused(int32 DefendingTeam)
{
	if (!HasAuthority())
	{
		return;
	}

	bBombPlanted = false;
	CompleteRound(DefendingTeam, FString(TEXT("Defuse")));
}

void ACOSnDGameMode::NotifyBombExploded(int32 AttackingTeam)
{
	if (!HasAuthority())
	{
		return;
	}

	bBombPlanted = false;
	CompleteRound(AttackingTeam, FString(TEXT("Bomb Exploded")));
}

void ACOSnDGameMode::StartRound()
{
	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		GS->SetMatchPhase(ECovertOpsMatchPhase::InProgress);
		GS->SetRemainingMatchTime(TimeLimitSeconds);
		CurrentAttackingTeam = ((GS->GetCurrentRound() + 1) % 2);
	}

	bBombPlanted = false;
	if (BombActor)
	{
		BombActor->ResetBombToSpawn();
	}

	BroadcastObjectiveStatus(FString::Printf(TEXT("Round start: Team %d attacking"), CurrentAttackingTeam));
}

void ACOSnDGameMode::EvaluateMatchPoint()
{
	ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>();
	if (!GS)
	{
		return;
	}

	const TArray<int32>& RoundWins = GS->GetRoundWins();
	if (RoundWins.Num() < 2)
	{
		return;
	}

	if (RoundWins[0] >= RoundsToWin)
	{
		EndMatchWithWinner(0, FString(TEXT("Round win limit reached")));
		return;
	}

	if (RoundWins[1] >= RoundsToWin)
	{
		EndMatchWithWinner(1, FString(TEXT("Round win limit reached")));
		return;
	}

	if (GS->GetCurrentRound() > MaxRounds)
	{
		const int32 WinningTeam = RoundWins[0] >= RoundWins[1] ? 0 : 1;
		EndMatchWithWinner(WinningTeam, FString(TEXT("Max rounds reached")));
		return;
	}

	FTimerDelegate NextRoundDelegate;
	NextRoundDelegate.BindUObject(this, &ACOSnDGameMode::StartRound);

	FTimerHandle NextRoundTimer;
	GetWorldTimerManager().SetTimer(NextRoundTimer, NextRoundDelegate, RoundTransitionDelay, false);
}

void ACOSnDGameMode::DiscoverBombObjectives()
{
	TArray<AActor*> BombActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombObjectiveActor::StaticClass(), BombActors);
	if (!BombActors.IsEmpty())
	{
		BombActor = Cast<ACOBombObjectiveActor>(BombActors[0]);
	}

	if (BombActor)
	{
		BombActor->OnBombPlanted.AddDynamic(this, &ACOSnDGameMode::OnBombPlantedInternal);
		BombActor->OnBombDefused.AddDynamic(this, &ACOSnDGameMode::OnBombDefusedInternal);
		BombActor->OnBombExploded.AddDynamic(this, &ACOSnDGameMode::OnBombExplodedInternal);
	}

	TArray<AActor*> SiteActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombSiteActor::StaticClass(), SiteActors);
	BombSites.Empty();
	for (AActor* Actor : SiteActors)
	{
		if (ACOBombSiteActor* Site = Cast<ACOBombSiteActor>(Actor))
		{
			BombSites.Add(Site);
		}
	}

	UE_LOG(LogCovertOps1, Log, TEXT("SnD objective discovery complete. Bomb=%s Sites=%d"), *GetNameSafe(BombActor), BombSites.Num());
}

void ACOSnDGameMode::OnBombPlantedInternal(ACOBombSiteActor* Site, APawn* Planter)
{
	const ACOMultiplayerPlayerState* PS = Planter ? Planter->GetPlayerState<ACOMultiplayerPlayerState>() : nullptr;
	const int32 PlantingTeam = PS ? PS->GetTeamId() : CurrentAttackingTeam;
	NotifyBombPlanted(PlantingTeam);

	const FString SiteLabel = Site ? Site->GetSiteId().ToString() : FString(TEXT("?"));
	BroadcastObjectiveStatus(FString::Printf(TEXT("Bomb planted at site %s"), *SiteLabel));
}

void ACOSnDGameMode::OnBombDefusedInternal(APawn* Defuser)
{
	const ACOMultiplayerPlayerState* PS = Defuser ? Defuser->GetPlayerState<ACOMultiplayerPlayerState>() : nullptr;
	const int32 DefendingTeam = PS ? PS->GetTeamId() : (1 - CurrentAttackingTeam);
	NotifyBombDefused(DefendingTeam);
}

void ACOSnDGameMode::OnBombExplodedInternal()
{
	NotifyBombExploded(CurrentAttackingTeam);
}

void ACOSnDGameMode::TickMatchClock()
{
	if (!HasAuthority())
	{
		return;
	}

	ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>();
	if (!GS)
	{
		return;
	}

	const int32 NewTime = FMath::Max(0, GS->GetRemainingMatchTime() - 1);
	GS->SetRemainingMatchTime(NewTime);

	if (NewTime == 0 && !bBombPlanted)
	{
		NotifyBombDefused(1 - CurrentAttackingTeam);
	}
}
