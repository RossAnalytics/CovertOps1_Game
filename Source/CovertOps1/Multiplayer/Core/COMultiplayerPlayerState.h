// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Multiplayer/Interfaces/PerkProvider.h"
#include "Multiplayer/Interfaces/ScorestreakProvider.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMultiplayerPlayerState.generated.h"

class UCOMultiplayerAbilitySystemComponent;
class UCOCombatAttributeSet;

UCLASS()
class COVERTOPS1_API ACOMultiplayerPlayerState : public APlayerState, public IAbilitySystemInterface, public IPerkProvider, public IScorestreakProvider
{
	GENERATED_BODY()

public:
	ACOMultiplayerPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void ApplyPerkSelection(const FPerkSelection& InSelection) override;
	virtual bool HasPerk(FName PerkId) const override;

	virtual void AddScorePoints(int32 Amount, const FName& SourceTag) override;
	virtual bool ConsumeScorestreak(FName StreakId) override;
	virtual const TArray<FScorestreakState>& GetScorestreakStates() const override;

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Loadout")
	void SetLoadoutSpec(const FLoadoutSpec& InLoadout);

	UFUNCTION(BlueprintPure, Category="Multiplayer|Loadout")
	const FLoadoutSpec& GetLoadoutSpec() const { return LoadoutSpec; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Perks")
	const FPerkSelection& GetPerkSelection() const { return PerkSelection; }

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Team")
	void SetTeamId(int32 InTeamId);

	UFUNCTION(BlueprintPure, Category="Multiplayer|Team")
	int32 GetTeamId() const { return TeamId; }

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Stats")
	void RegisterKill();

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Stats")
	void RegisterDeath();

	UFUNCTION(BlueprintPure, Category="Multiplayer|Stats")
	int32 GetKills() const { return Kills; }

	UFUNCTION(BlueprintPure, Category="Multiplayer|Stats")
	int32 GetDeaths() const { return Deaths; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Multiplayer|GAS")
	TObjectPtr<UCOMultiplayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCOCombatAttributeSet> CombatAttributeSet;

	UPROPERTY(ReplicatedUsing=OnRep_LoadoutSpec, EditAnywhere, BlueprintReadOnly, Category="Multiplayer|Loadout")
	FLoadoutSpec LoadoutSpec;

	UPROPERTY(ReplicatedUsing=OnRep_PerkSelection, EditAnywhere, BlueprintReadOnly, Category="Multiplayer|Perks")
	FPerkSelection PerkSelection;

	UPROPERTY(ReplicatedUsing=OnRep_ScorestreakStates, EditAnywhere, BlueprintReadOnly, Category="Multiplayer|Scorestreaks")
	TArray<FScorestreakState> ScorestreakStates;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Multiplayer|Team")
	int32 TeamId = INDEX_NONE;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Multiplayer|Stats")
	int32 Kills = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Multiplayer|Stats")
	int32 Deaths = 0;

protected:
	UFUNCTION()
	void OnRep_LoadoutSpec();

	UFUNCTION()
	void OnRep_PerkSelection();

	UFUNCTION()
	void OnRep_ScorestreakStates();

	void RecomputeScorestreakUnlocks();
};
