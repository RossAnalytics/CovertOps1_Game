// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Components/CovertOpsSpawnDirectorComponent.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCovertOpsSpawnDirectorComponent::UCovertOpsSpawnDirectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FTransform UCovertOpsSpawnDirectorComponent::ChooseSpawnTransform(AController* RequestingController) const
{
	if (APlayerStart* BestStart = SelectBestPlayerStart(RequestingController))
	{
		return BestStart->GetActorTransform();
	}

	return FTransform::Identity;
}

APlayerStart* UCovertOpsSpawnDirectorComponent::SelectBestPlayerStart(AController* RequestingController) const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	TArray<AActor*> SpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnActors);

	if (SpawnActors.IsEmpty())
	{
		return nullptr;
	}

	int32 RequestTeam = INDEX_NONE;
	if (RequestingController)
	{
		if (const ACOMultiplayerPlayerState* PS = RequestingController->GetPlayerState<ACOMultiplayerPlayerState>())
		{
			RequestTeam = PS->GetTeamId();
		}
	}

	APlayerStart* BestStart = nullptr;
	float BestScore = -1.0f;

	for (AActor* SpawnActor : SpawnActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(SpawnActor);
		if (!PlayerStart)
		{
			continue;
		}

		float NearestEnemyDistanceSq = FLT_MAX;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			const AController* OtherController = It->Get();
			if (!OtherController || OtherController == RequestingController)
			{
				continue;
			}

			const ACOMultiplayerPlayerState* OtherPS = OtherController->GetPlayerState<ACOMultiplayerPlayerState>();
			if (!OtherPS || OtherPS->GetTeamId() == RequestTeam)
			{
				continue;
			}

			const APawn* OtherPawn = OtherController->GetPawn();
			if (!OtherPawn)
			{
				continue;
			}

			const float DistSq = FVector::DistSquared(OtherPawn->GetActorLocation(), PlayerStart->GetActorLocation());
			NearestEnemyDistanceSq = FMath::Min(NearestEnemyDistanceSq, DistSq);
		}

		const float StartScore = FMath::Sqrt(NearestEnemyDistanceSq);
		if (StartScore > BestScore && StartScore >= MinEnemyDistance)
		{
			BestScore = StartScore;
			BestStart = PlayerStart;
		}
	}

	if (!BestStart)
	{
		BestStart = Cast<APlayerStart>(SpawnActors[FMath::RandRange(0, SpawnActors.Num() - 1)]);
	}

	return BestStart;
}
