// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/GAS/COCombatAttributeSet.h"
#include "Net/UnrealNetwork.h"

UCOCombatAttributeSet::UCOCombatAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitStamina(100.0f);
	InitMaxStamina(100.0f);
	InitAccuracy(1.0f);
}

void UCOCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCOCombatAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCOCombatAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCOCombatAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCOCombatAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCOCombatAttributeSet, Accuracy, COND_None, REPNOTIFY_Always);
}

void UCOCombatAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCOCombatAttributeSet, Health, OldHealth);
}

void UCOCombatAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCOCombatAttributeSet, MaxHealth, OldMaxHealth);
}

void UCOCombatAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCOCombatAttributeSet, Stamina, OldStamina);
}

void UCOCombatAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCOCombatAttributeSet, MaxStamina, OldMaxStamina);
}

void UCOCombatAttributeSet::OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCOCombatAttributeSet, Accuracy, OldAccuracy);
}
