// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Weapons/COHitscanWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

ACOHitscanWeapon::ACOHitscanWeapon()
{
	PointDamageType = UDamageType::StaticClass();
}

void ACOHitscanWeapon::FireProjectile(const FVector& TargetLocation)
{
	if (!WeaponOwner || !GetWorld())
	{
		return;
	}

	const FTransform ProjectileTransform = CalculateProjectileSpawnTransform(TargetLocation);
	const FVector TraceStart = ProjectileTransform.GetLocation();
	const FVector Direction = (TargetLocation - TraceStart).GetSafeNormal();
	const FVector TraceEnd = TraceStart + ((Direction.IsNearlyZero() ? GetActorForwardVector() : Direction) * MaxTraceDistance);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HitscanTrace), false);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (HitResult.bBlockingHit)
	{
		const float Distance = FVector::Dist(TraceStart, HitResult.ImpactPoint);
		const float Damage = ComputeDamageForDistance(Distance);

		if (AActor* HitActor = HitResult.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(
				HitActor,
				Damage,
				(Direction.IsNearlyZero() ? GetActorForwardVector() : Direction),
				HitResult,
				PawnOwner ? PawnOwner->GetController() : nullptr,
				this,
				PointDamageType);
		}

		if (UPrimitiveComponent* HitComponent = HitResult.GetComponent())
		{
			if (HitComponent->IsSimulatingPhysics())
			{
				const FVector ImpulseDirection = Direction.IsNearlyZero() ? GetActorForwardVector() : Direction;
				HitComponent->AddImpulseAtLocation(ImpulseDirection * ImpactImpulse, HitResult.ImpactPoint);
			}
		}

		BP_OnHitscanImpact(HitResult);
	}

	WeaponOwner->PlayFiringMontage(FiringMontage);
	WeaponOwner->AddWeaponRecoil(FiringRecoil);

	--CurrentBullets;
	if (CurrentBullets <= 0)
	{
		CurrentBullets = MagazineSize;
	}

	WeaponOwner->UpdateWeaponHUD(CurrentBullets, MagazineSize);
}

float ACOHitscanWeapon::ComputeDamageForDistance(float Distance) const
{
	if (Distance <= MaxDamageRange)
	{
		return MaxDamage;
	}

	if (Distance >= MinDamageRange)
	{
		return MinDamage;
	}

	const float Alpha = (Distance - MaxDamageRange) / FMath::Max(1.0f, MinDamageRange - MaxDamageRange);
	return FMath::Lerp(MaxDamage, MinDamage, Alpha);
}
