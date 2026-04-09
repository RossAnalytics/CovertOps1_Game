// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "CovertOps1.h"
#include "Multiplayer/GAS/COCombatAttributeSet.h"
#include "Multiplayer/GAS/COMultiplayerAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

ACOMultiplayerPlayerState::ACOMultiplayerPlayerState()
{
	bReplicates = true;
	SetNetUpdateFrequency(30.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UCOMultiplayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	CombatAttributeSet = CreateDefaultSubobject<UCOCombatAttributeSet>(TEXT("CombatAttributeSet"));

	ScorestreakStates =
	{
		{ FName(TEXT("ReconDrone")), 300, false, false },
		{ FName(TEXT("RCDetonator")), 400, false, false },
		{ FName(TEXT("PrecisionStrike")), 500, false, false },
		{ FName(TEXT("AttackChopper")), 700, false, false },
		{ FName(TEXT("GuardDog")), 900, false, false },
		{ FName(TEXT("Gunship")), 1100, false, false }
	};
}

void ACOMultiplayerPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACOMultiplayerPlayerState, LoadoutSpec);
	DOREPLIFETIME(ACOMultiplayerPlayerState, PerkSelection);
	DOREPLIFETIME(ACOMultiplayerPlayerState, ScorestreakStates);
	DOREPLIFETIME(ACOMultiplayerPlayerState, TeamId);
	DOREPLIFETIME(ACOMultiplayerPlayerState, Kills);
	DOREPLIFETIME(ACOMultiplayerPlayerState, Deaths);
}

UAbilitySystemComponent* ACOMultiplayerPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACOMultiplayerPlayerState::ApplyPerkSelection(const FPerkSelection& InSelection)
{
	if (!HasAuthority())
	{
		return;
	}

	PerkSelection = InSelection;
	OnRep_PerkSelection();
}

bool ACOMultiplayerPlayerState::HasPerk(FName PerkId) const
{
	return PerkSelection.Tier1Perk == PerkId
		|| PerkSelection.Tier2Perk == PerkId
		|| PerkSelection.Tier3Perk == PerkId;
}

void ACOMultiplayerPlayerState::AddScorePoints(int32 Amount, const FName& SourceTag)
{
	if (!HasAuthority())
	{
		return;
	}

	SetScore(GetScore() + Amount);
	RecomputeScorestreakUnlocks();

	UE_LOG(LogCovertOps1, Verbose, TEXT("Added %d score to %s from %s. Total: %.0f"),
		Amount, *GetPlayerName(), *SourceTag.ToString(), GetScore());
}

bool ACOMultiplayerPlayerState::ConsumeScorestreak(FName StreakId)
{
	if (!HasAuthority())
	{
		return false;
	}

	for (FScorestreakState& Entry : ScorestreakStates)
	{
		if (Entry.StreakId == StreakId && Entry.bReadyToUse)
		{
			Entry.bReadyToUse = false;
			OnRep_ScorestreakStates();
			return true;
		}
	}

	return false;
}

const TArray<FScorestreakState>& ACOMultiplayerPlayerState::GetScorestreakStates() const
{
	return ScorestreakStates;
}

void ACOMultiplayerPlayerState::SetLoadoutSpec(const FLoadoutSpec& InLoadout)
{
	if (!HasAuthority())
	{
		return;
	}

	LoadoutSpec = InLoadout;
	OnRep_LoadoutSpec();
}

void ACOMultiplayerPlayerState::SetTeamId(int32 InTeamId)
{
	if (!HasAuthority())
	{
		return;
	}

	TeamId = InTeamId;
}

void ACOMultiplayerPlayerState::RegisterKill()
{
	if (!HasAuthority())
	{
		return;
	}

	++Kills;
	AddScorePoints(100, FName(TEXT("Kill")));
}

void ACOMultiplayerPlayerState::RegisterDeath()
{
	if (!HasAuthority())
	{
		return;
	}

	++Deaths;
}

void ACOMultiplayerPlayerState::OnRep_LoadoutSpec()
{
}

void ACOMultiplayerPlayerState::OnRep_PerkSelection()
{
}

void ACOMultiplayerPlayerState::OnRep_ScorestreakStates()
{
}

void ACOMultiplayerPlayerState::RecomputeScorestreakUnlocks()
{
	const int32 CurrentScore = FMath::RoundToInt(GetScore());
	for (FScorestreakState& Entry : ScorestreakStates)
	{
		Entry.bUnlocked = CurrentScore >= Entry.ScoreRequired;
		Entry.bReadyToUse = Entry.bUnlocked;
	}
}
