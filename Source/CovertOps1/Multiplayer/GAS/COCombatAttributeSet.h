// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "COCombatAttributeSet.generated.h"

#define CO_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class COVERTOPS1_API UCOCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCOCombatAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="Attributes")
	FGameplayAttributeData Health;
	CO_ATTRIBUTE_ACCESSORS(UCOCombatAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="Attributes")
	FGameplayAttributeData MaxHealth;
	CO_ATTRIBUTE_ACCESSORS(UCOCombatAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Stamina, Category="Attributes")
	FGameplayAttributeData Stamina;
	CO_ATTRIBUTE_ACCESSORS(UCOCombatAttributeSet, Stamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxStamina, Category="Attributes")
	FGameplayAttributeData MaxStamina;
	CO_ATTRIBUTE_ACCESSORS(UCOCombatAttributeSet, MaxStamina);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Accuracy, Category="Attributes")
	FGameplayAttributeData Accuracy;
	CO_ATTRIBUTE_ACCESSORS(UCOCombatAttributeSet, Accuracy);

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldStamina) const;

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const;

	UFUNCTION()
	void OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy) const;
};
