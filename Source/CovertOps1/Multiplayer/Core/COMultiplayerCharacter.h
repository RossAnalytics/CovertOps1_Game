// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "COMultiplayerCharacter.generated.h"

class UInputAction;
class ACOFlagObjectiveActor;

UCLASS()
class COVERTOPS1_API ACOMultiplayerCharacter : public AShooterCharacter
{
	GENERATED_BODY()

public:
	ACOMultiplayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTimeSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SlideAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ProneAction;

	UPROPERTY(ReplicatedUsing=OnRep_Stance, BlueprintReadOnly, Category="Movement")
	ECovertOpsStance CurrentStance = ECovertOpsStance::Standing;

	UPROPERTY(ReplicatedUsing=OnRep_IsSprinting, BlueprintReadOnly, Category="Movement")
	bool bIsSprinting = false;

	UPROPERTY(ReplicatedUsing=OnRep_Stamina, BlueprintReadOnly, Category="Movement")
	float CurrentStamina = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_NetworkHealth, BlueprintReadOnly, Category="Health")
	float NetworkHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_CarriedFlag, BlueprintReadOnly, Category="CTF")
	TObjectPtr<ACOFlagObjectiveActor> CarriedFlag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Speeds")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Speeds")
	float SprintSpeed = 850.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Speeds")
	float CrouchSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Speeds")
	float ProneSpeed = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Speeds")
	float SlideSpeed = 920.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Stamina")
	float SprintDrainPerSecond = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement|Stamina")
	float StaminaRegenPerSecond = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	float RegenDelaySeconds = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
	float RegenPerSecond = 20.0f;

protected:
	FTimerHandle SlideTransitionTimer;
	double LastDamageTimeSeconds = 0.0;

protected:
	UFUNCTION()
	void OnRep_Stance();

	UFUNCTION()
	void OnRep_IsSprinting();

	UFUNCTION()
	void OnRep_Stamina();

	UFUNCTION()
	void OnRep_NetworkHealth();

	UFUNCTION()
	void OnRep_CarriedFlag();

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool bNewSprinting);

	UFUNCTION(Server, Reliable)
	void ServerSetStance(ECovertOpsStance NewStance);

	void StartSprinting();
	void StopSprinting();
	void TriggerSlide();
	void ToggleProne();
	void CompleteSlide();
	void UpdateMovementState();

public:
	UFUNCTION(BlueprintPure, Category="CTF")
	ACOFlagObjectiveActor* GetCarriedFlag() const { return CarriedFlag; }

	void SetCarriedFlag(ACOFlagObjectiveActor* InFlag);

	UFUNCTION(BlueprintPure, Category="Combat")
	bool WasDamagedRecently(float WithinSeconds) const;
};
