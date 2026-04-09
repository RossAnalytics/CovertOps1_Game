// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "COFlagCaptureZoneActor.generated.h"

class USphereComponent;
class ACOFlagObjectiveActor;

UCLASS()
class COVERTOPS1_API ACOFlagCaptureZoneActor : public AActor
{
	GENERATED_BODY()

public:
	ACOFlagCaptureZoneActor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="CTF")
	void ConfigureFlags(ACOFlagObjectiveActor* InFriendlyFlag, ACOFlagObjectiveActor* InEnemyFlag);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> CaptureSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF")
	int32 ZoneTeamId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF")
	TObjectPtr<ACOFlagObjectiveActor> FriendlyFlag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF")
	TObjectPtr<ACOFlagObjectiveActor> EnemyFlag;

protected:
	UFUNCTION()
	void OnCaptureSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
