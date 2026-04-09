// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Shooter/Weapons/ShooterWeapon.h"
#include "COHitscanWeapon.generated.h"

UCLASS(Abstract)
class COVERTOPS1_API ACOHitscanWeapon : public AShooterWeapon
{
	GENERATED_BODY()

public:
	ACOHitscanWeapon();

protected:
	UPROPERTY(EditAnywhere, Category="Hitscan")
	float MaxTraceDistance = 20000.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	float MaxDamage = 35.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	float MinDamage = 15.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	float MaxDamageRange = 1800.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	float MinDamageRange = 5000.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	float ImpactImpulse = 500.0f;

	UPROPERTY(EditAnywhere, Category="Hitscan")
	TSubclassOf<UDamageType> PointDamageType;

protected:
	virtual void FireProjectile(const FVector& TargetLocation) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Hitscan")
	void BP_OnHitscanImpact(const FHitResult& HitResult);

	float ComputeDamageForDistance(float Distance) const;
};
