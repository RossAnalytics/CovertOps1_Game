// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Modes/COMultiplayerGameMode.h"
#include "CovertOps1.h"
#include "Multiplayer/Components/CovertOpsSpawnDirectorComponent.h"
#include "Multiplayer/Core/COMultiplayerCharacter.h"
#include "Multiplayer/Core/COMultiplayerGameState.h"
#include "Multiplayer/Core/COMultiplayerPlayerController.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/UI/COMultiplayerHUD.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "TimerManager.h"

ACOMultiplayerGameMode::ACOMultiplayerGameMode()
{
	GameStateClass = ACOMultiplayerGameState::StaticClass();
	PlayerStateClass = ACOMultiplayerPlayerState::StaticClass();
	PlayerControllerClass = ACOMultiplayerPlayerController::StaticClass();
	DefaultPawnClass = ACOMultiplayerCharacter::StaticClass();
	HUDClass = ACOMultiplayerHUD::StaticClass();

	SpawnDirector = CreateDefaultSubobject<UCovertOpsSpawnDirectorComponent>(TEXT("SpawnDirector"));
}

void ACOMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	MatchStartTime = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(MatchClockTimer, this, &ACOMultiplayerGameMode::TickMatchClock, 1.0f, true);
	OnModeStarted();
}

void ACOMultiplayerGameMode::InitGameState()
{
	Super::InitGameState();

	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		GS->SetMatchMode(MatchMode);
		GS->SetMatchPhase(ECovertOpsMatchPhase::InProgress);
		GS->SetRemainingMatchTime(TimeLimitSeconds);
	}
}

void ACOMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ACOMultiplayerPlayerState* PS = NewPlayer ? NewPlayer->GetPlayerState<ACOMultiplayerPlayerState>() : nullptr)
	{
		PS->SetTeamId(NextTeamAssignment);
		NextTeamAssignment = (NextTeamAssignment + 1) % 2;
	}
}

AActor* ACOMultiplayerGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (SpawnDirector)
	{
		if (APlayerStart* Start = SpawnDirector->SelectBestPlayerStart(Player))
		{
			return Start;
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ACOMultiplayerGameMode::RegisterElimination(ACOMultiplayerPlayerState* KillerPS, ACOMultiplayerPlayerState* VictimPS, int32 VictimTeamId, bool bObjectiveKill)
{
	if (!HasAuthority())
	{
		return;
	}

	if (VictimPS)
	{
		VictimPS->RegisterDeath();
	}

	int32 ScoringTeam = INDEX_NONE;
	if (KillerPS && KillerPS != VictimPS)
	{
		KillerPS->RegisterKill();
		KillerPS->AddScorePoints(bObjectiveKill ? 150 : 100, bObjectiveKill ? FName(TEXT("ObjectiveKill")) : FName(TEXT("Kill")));
		ScoringTeam = KillerPS->GetTeamId();
	}
	else if (VictimTeamId != INDEX_NONE)
	{
		ScoringTeam = VictimTeamId;
	}

	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		if (ScoringTeam != INDEX_NONE)
		{
			GS->AddTeamScore(ScoringTeam, 1);

			if (!bRoundBased && ScoreLimit > 0 && GS->GetTeamScore(ScoringTeam) >= ScoreLimit)
			{
				EndMatchWithWinner(ScoringTeam, FString(TEXT("Score limit reached")));
			}
		}
	}

	if (VictimPS)
	{
		AController* VictimController = nullptr;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AController* Controller = It->Get();
			if (Controller && Controller->PlayerState == VictimPS)
			{
				VictimController = Controller;
				break;
			}
		}

		ScheduleRespawn(VictimController, RespawnDelaySeconds);
	}

	const FString KillerName = KillerPS ? KillerPS->GetPlayerName() : FString(TEXT("Environment"));
	const FString VictimName = VictimPS ? VictimPS->GetPlayerName() : FString(TEXT("Unknown"));
	const FString KillfeedMessage = FString::Printf(TEXT("%s eliminated %s"), *KillerName, *VictimName);
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACOMultiplayerPlayerController* MPC = Cast<ACOMultiplayerPlayerController>(It->Get()))
		{
			MPC->ClientNotifyKillfeedEvent(KillfeedMessage);
		}
	}
}

void ACOMultiplayerGameMode::EndMatchWithWinner(int32 WinningTeam, const FString& Reason)
{
	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		GS->SetMatchPhase(ECovertOpsMatchPhase::Completed);
		GS->SetRemainingMatchTime(0);
	}

	GetWorldTimerManager().ClearTimer(MatchClockTimer);

	UE_LOG(LogCovertOps1, Log, TEXT("Match complete. Winner Team=%d Reason=%s"), WinningTeam, *Reason);
}

bool ACOMultiplayerGameMode::CanControllerRespawn(AController* Controller) const
{
	return bAllowRespawns && Controller != nullptr;
}

void ACOMultiplayerGameMode::OnModeStarted()
{
}

void ACOMultiplayerGameMode::TickMatchClock()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ACOMultiplayerGameState* GS = GetGameState<ACOMultiplayerGameState>())
	{
		const int32 NewTime = FMath::Max(0, GS->GetRemainingMatchTime() - 1);
		GS->SetRemainingMatchTime(NewTime);

		if (NewTime == 0)
		{
			int32 BestTeam = INDEX_NONE;
			int32 BestScore = MIN_int32;
			for (int32 TeamId = 0; TeamId < 2; ++TeamId)
			{
				const int32 TeamScore = GS->GetTeamScore(TeamId);
				if (TeamScore > BestScore)
				{
					BestScore = TeamScore;
					BestTeam = TeamId;
				}
			}

			EndMatchWithWinner(BestTeam, FString(TEXT("Time limit reached")));
		}
	}
}

void ACOMultiplayerGameMode::ScheduleRespawn(AController* Controller, float DelaySeconds)
{
	if (!CanControllerRespawn(Controller))
	{
		return;
	}

	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindLambda([this, Controller]()
	{
		if (IsValid(this) && IsValid(Controller) && CanControllerRespawn(Controller))
		{
			RestartPlayer(Controller);
		}
	});

	FTimerHandle RespawnHandle;
	GetWorldTimerManager().SetTimer(RespawnHandle, RespawnDelegate, DelaySeconds, false);
}

void ACOMultiplayerGameMode::BroadcastObjectiveStatus(const FString& StatusText) const
{
	if (!GetWorld())
	{
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACOMultiplayerPlayerController* MPC = Cast<ACOMultiplayerPlayerController>(It->Get()))
		{
			MPC->ClientNotifyObjectiveStatus(StatusText);
		}
	}
}
