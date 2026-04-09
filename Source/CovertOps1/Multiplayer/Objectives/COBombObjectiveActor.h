// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "COBombObjectiveActor.generated.h"

class USphereComponent;
class ACOBombSiteActor;
class USceneComponent;
class UStaticMeshComponent;
class ACOMultiplayerCharacter;

UENUM(BlueprintType)
enum class ECOBombState : uint8
{
	AtSpawn UMETA(DisplayName = "At Spawn"),
	Carried UMETA(DisplayName = "Carried"),
	Dropped UMETA(DisplayName = "Dropped"),
	Planted UMETA(DisplayName = "Planted"),
	Defused UMETA(DisplayName = "Defused"),
	Exploded UMETA(DisplayName = "Exploded")
};

UENUM(BlueprintType)
enum class ECOBombInteractionType : uint8
{
	None UMETA(DisplayName = "None"),
	Planting UMETA(DisplayName = "Planting"),
	Defusing UMETA(DisplayName = "Defusing")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCOBombPlantedEvent, ACOBombSiteActor*, Site, APawn*, Planter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCOBombSimplePawnEvent, APawn*, InstigatorPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCOBombSimpleEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCOBombInteractionProgressEvent, ECOBombInteractionType, InteractionType, APawn*, InteractingPawn, float, NormalizedProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCOBombInteractionEvent, ECOBombInteractionType, InteractionType, APawn*, InteractingPawn);

UCLASS()
class COVERTOPS1_API ACOBombObjectiveActor : public AActor
{
	GENERATED_BODY()

public:
	ACOBombObjectiveActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="SnD")
	bool PickupBomb(APawn* NewCarrier);

	UFUNCTION(BlueprintCallable, Category="SnD")
	bool PlantBomb(ACOBombSiteActor* Site, APawn* Planter);

	UFUNCTION(BlueprintCallable, Category="SnD")
	bool StartPlanting(APawn* Planter, ACOBombSiteActor* Site);

	UFUNCTION(BlueprintCallable, Category="SnD")
	bool StartDefuse(APawn* Defuser);

	UFUNCTION(BlueprintCallable, Category="SnD")
	void CancelDefuse();

	UFUNCTION(BlueprintCallable, Category="SnD")
	void StopInteractionForPawn(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category="SnD")
	void DropBomb(const FVector& DropLocation);

	UFUNCTION(BlueprintCallable, Category="SnD")
	void ResetBombToSpawn();

	UFUNCTION(BlueprintPure, Category="SnD")
	ECOBombState GetBombState() const { return BombState; }

	UFUNCTION(BlueprintPure, Category="SnD")
	APawn* GetCarrierPawn() const { return CarrierPawn; }

	UFUNCTION(BlueprintPure, Category="SnD")
	ACOBombSiteActor* GetPlantedSite() const { return PlantedSite; }

	UFUNCTION(BlueprintPure, Category="SnD")
	bool IsInteractionInProgress() const { return InteractionType != ECOBombInteractionType::None; }

	UFUNCTION(BlueprintPure, Category="SnD")
	ECOBombInteractionType GetInteractionType() const { return InteractionType; }

	UFUNCTION(BlueprintPure, Category="SnD")
	float GetInteractionProgress() const { return InteractionProgress; }

	UFUNCTION(BlueprintPure, Category="SnD")
	APawn* GetInteractingPawn() const { return ActiveInteractor; }

public:
	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombPlantedEvent OnBombPlanted;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombSimplePawnEvent OnBombDefused;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombSimpleEvent OnBombExploded;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombSimplePawnEvent OnBombPickedUp;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombInteractionProgressEvent OnBombInteractionProgress;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombInteractionEvent OnBombInteractionStarted;

	UPROPERTY(BlueprintAssignable, Category="SnD")
	FCOBombInteractionEvent OnBombInteractionCancelled;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> BombMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> BombInteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	float FuseDurationSeconds = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	float DefuseDurationSeconds = 7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	float PlantDurationSeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	float MovementCancelSpeed = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SnD")
	float DamageCancelWindowSeconds = 0.2f;

	UPROPERTY(ReplicatedUsing=OnRep_BombState, BlueprintReadOnly, Category="SnD")
	ECOBombState BombState = ECOBombState::AtSpawn;

	UPROPERTY(ReplicatedUsing=OnRep_CarrierPawn, BlueprintReadOnly, Category="SnD")
	TObjectPtr<APawn> CarrierPawn;

	UPROPERTY(ReplicatedUsing=OnRep_PlantedSite, BlueprintReadOnly, Category="SnD")
	TObjectPtr<ACOBombSiteActor> PlantedSite;

	UPROPERTY(ReplicatedUsing=OnRep_InteractionState, BlueprintReadOnly, Category="SnD")
	ECOBombInteractionType InteractionType = ECOBombInteractionType::None;

	UPROPERTY(ReplicatedUsing=OnRep_InteractionState, BlueprintReadOnly, Category="SnD")
	float InteractionProgress = 0.0f;

	UPROPERTY(ReplicatedUsing=OnRep_InteractionState, BlueprintReadOnly, Category="SnD")
	TObjectPtr<APawn> ActiveInteractor;

	UPROPERTY(ReplicatedUsing=OnRep_InteractionState, BlueprintReadOnly, Category="SnD")
	TObjectPtr<ACOBombSiteActor> ActiveInteractionSite;

protected:
	FTransform SpawnTransform;
	FTimerHandle FuseTimer;
	FTimerHandle DefuseTimer;
	float InteractionElapsedSeconds = 0.0f;
	float ActiveInteractionDuration = 0.0f;

protected:
	UFUNCTION()
	void OnRep_BombState();

	UFUNCTION()
	void OnRep_CarrierPawn();

	UFUNCTION()
	void OnRep_PlantedSite();

	UFUNCTION()
	void OnRep_InteractionState();

	void RefreshAttachment();
	void CompleteDefuse();
	void OnFuseExpired();
	bool ValidateInteraction(APawn* Pawn, ACOBombSiteActor* Site, ECOBombInteractionType InType) const;
	void ClearInteraction(ECOBombInteractionType CancelledType);
	void StartInteraction(ECOBombInteractionType InType, APawn* Interactor, ACOBombSiteActor* Site, float DurationSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category="SnD|FX")
	void BP_OnBombInteractionChanged(ECOBombInteractionType InType, float Progress, APawn* InteractingPawn);

	UFUNCTION(BlueprintImplementableEvent, Category="SnD|FX")
	void BP_OnBombStateChanged(ECOBombState InState, APawn* InCarrier, ACOBombSiteActor* InPlantedSite);
};
