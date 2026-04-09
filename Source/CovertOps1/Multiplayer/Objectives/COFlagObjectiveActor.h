// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "COFlagObjectiveActor.generated.h"

class USphereComponent;
class ACOMultiplayerCharacter;

UENUM(BlueprintType)
enum class ECOFlagState : uint8
{
	AtHome UMETA(DisplayName = "At Home"),
	Carried UMETA(DisplayName = "Carried"),
	Dropped UMETA(DisplayName = "Dropped")
};

UCLASS()
class COVERTOPS1_API ACOFlagObjectiveActor : public AActor
{
	GENERATED_BODY()

public:
	ACOFlagObjectiveActor();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="CTF")
	bool TryPickup(APawn* NewCarrier);

	UFUNCTION(BlueprintCallable, Category="CTF")
	void DropFlag(const FVector& DropLocation, bool bCaptured);

	UFUNCTION(BlueprintCallable, Category="CTF")
	void ReturnToBase();

	UFUNCTION(BlueprintCallable, Category="CTF")
	void CaptureBy(APawn* CapturingPawn);

	UFUNCTION(BlueprintPure, Category="CTF")
	bool IsAtHome() const { return FlagState == ECOFlagState::AtHome; }

	UFUNCTION(BlueprintPure, Category="CTF")
	bool IsCarried() const { return FlagState == ECOFlagState::Carried; }

	UFUNCTION(BlueprintPure, Category="CTF")
	bool IsDropped() const { return FlagState == ECOFlagState::Dropped; }

	UFUNCTION(BlueprintPure, Category="CTF")
	int32 GetFlagTeamId() const { return FlagTeamId; }

	UFUNCTION(BlueprintPure, Category="CTF")
	APawn* GetCarrierPawn() const { return CarrierPawn; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> FlagMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF")
	int32 FlagTeamId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="CTF")
	float AutoReturnDelaySeconds = 12.0f;

	UPROPERTY(ReplicatedUsing=OnRep_FlagState, BlueprintReadOnly, Category="CTF")
	ECOFlagState FlagState = ECOFlagState::AtHome;

	UPROPERTY(ReplicatedUsing=OnRep_CarrierPawn, BlueprintReadOnly, Category="CTF")
	TObjectPtr<APawn> CarrierPawn;

protected:
	FTransform HomeTransform;
	FTimerHandle AutoReturnTimer;

protected:
	UFUNCTION()
	void OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRep_FlagState();

	UFUNCTION()
	void OnRep_CarrierPawn();

	void RefreshAttachment();
	void SetCarrierCharacterFlag(ACOMultiplayerCharacter* Character, bool bOwnsFlag);
	void OnAutoReturnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category="CTF|Presentation")
	void BP_OnFlagStateChanged(ECOFlagState NewState, APawn* InCarrier, bool bCaptured);
};
