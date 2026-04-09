// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMultiplayerCharacter.h"
#include "CovertOps1.h"
#include "Multiplayer/Objectives/COFlagObjectiveActor.h"
#include "Multiplayer/Objectives/COBombObjectiveActor.h"
#include "Multiplayer/Core/COMultiplayerPlayerState.h"
#include "Multiplayer/Modes/COMultiplayerGameMode.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

ACOMultiplayerCharacter::ACOMultiplayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
}

void ACOMultiplayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	NetworkHealth = MaxHP;
	LastDamageTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	UpdateMovementState();
}

void ACOMultiplayerCharacter::Tick(float DeltaTimeSeconds)
{
	Super::Tick(DeltaTimeSeconds);

	if (!HasAuthority())
	{
		return;
	}

	if (bIsSprinting)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - (SprintDrainPerSecond * DeltaTimeSeconds), 0.0f, MaxStamina);
		if (CurrentStamina <= KINDA_SMALL_NUMBER)
		{
			bIsSprinting = false;
			OnRep_IsSprinting();
		}
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenPerSecond * DeltaTimeSeconds), 0.0f, MaxStamina);
	}

	if (!IsDead() && CurrentHP < MaxHP)
	{
		const double Elapsed = GetWorld()->GetTimeSeconds() - LastDamageTimeSeconds;
		if (Elapsed >= RegenDelaySeconds)
		{
			CurrentHP = FMath::Clamp(CurrentHP + (RegenPerSecond * DeltaTimeSeconds), 0.0f, MaxHP);
		}
	}

	NetworkHealth = CurrentHP;
}

void ACOMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACOMultiplayerCharacter::StartSprinting);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACOMultiplayerCharacter::StopSprinting);
		}

		if (SlideAction)
		{
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ACOMultiplayerCharacter::TriggerSlide);
		}

		if (ProneAction)
		{
			EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Started, this, &ACOMultiplayerCharacter::ToggleProne);
		}
	}
}

float ACOMultiplayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const bool bWasDead = IsDead();
	const float DealtDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	LastDamageTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : LastDamageTimeSeconds;
	NetworkHealth = CurrentHP;

	if (HasAuthority() && !bWasDead && IsDead())
	{
		if (CarriedFlag)
		{
			CarriedFlag->DropFlag(GetActorLocation(), false);
		}

		TArray<AActor*> BombActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOBombObjectiveActor::StaticClass(), BombActors);
		for (AActor* BombActor : BombActors)
		{
			if (ACOBombObjectiveActor* Bomb = Cast<ACOBombObjectiveActor>(BombActor))
			{
				if (Bomb->GetBombState() == ECOBombState::Carried && Bomb->GetCarrierPawn() == this)
				{
					Bomb->DropBomb(GetActorLocation());
				}

				Bomb->StopInteractionForPawn(this);
			}
		}

		ACOMultiplayerPlayerState* VictimPS = Cast<ACOMultiplayerPlayerState>(GetPlayerState());
		ACOMultiplayerPlayerState* KillerPS = EventInstigator ? Cast<ACOMultiplayerPlayerState>(EventInstigator->PlayerState) : nullptr;
		const int32 VictimTeam = VictimPS ? VictimPS->GetTeamId() : INDEX_NONE;

		if (ACOMultiplayerGameMode* GM = GetWorld() ? Cast<ACOMultiplayerGameMode>(GetWorld()->GetAuthGameMode()) : nullptr)
		{
			GM->RegisterElimination(KillerPS, VictimPS, VictimTeam, false);
		}
	}

	return DealtDamage;
}

void ACOMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACOMultiplayerCharacter, CurrentStance);
	DOREPLIFETIME(ACOMultiplayerCharacter, bIsSprinting);
	DOREPLIFETIME(ACOMultiplayerCharacter, CurrentStamina);
	DOREPLIFETIME(ACOMultiplayerCharacter, NetworkHealth);
	DOREPLIFETIME(ACOMultiplayerCharacter, CarriedFlag);
}

void ACOMultiplayerCharacter::OnRep_Stance()
{
	UpdateMovementState();
}

void ACOMultiplayerCharacter::OnRep_IsSprinting()
{
	UpdateMovementState();
}

void ACOMultiplayerCharacter::OnRep_Stamina()
{
}

void ACOMultiplayerCharacter::OnRep_NetworkHealth()
{
	if (MaxHP > 0.0f)
	{
		OnDamaged.Broadcast(NetworkHealth / MaxHP);
	}
}

void ACOMultiplayerCharacter::OnRep_CarriedFlag()
{
}

void ACOMultiplayerCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
	bIsSprinting = bNewSprinting && CurrentStamina > KINDA_SMALL_NUMBER && CurrentStance == ECovertOpsStance::Standing;
	UpdateMovementState();
}

void ACOMultiplayerCharacter::ServerSetStance_Implementation(ECovertOpsStance NewStance)
{
	CurrentStance = NewStance;
	if (CurrentStance != ECovertOpsStance::Standing)
	{
		bIsSprinting = false;
	}

	UpdateMovementState();
}

void ACOMultiplayerCharacter::StartSprinting()
{
	if (CurrentStance != ECovertOpsStance::Standing || CurrentStamina <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (HasAuthority())
	{
		bIsSprinting = true;
		UpdateMovementState();
	}
	else
	{
		ServerSetSprinting(true);
	}
}

void ACOMultiplayerCharacter::StopSprinting()
{
	if (HasAuthority())
	{
		bIsSprinting = false;
		UpdateMovementState();
	}
	else
	{
		ServerSetSprinting(false);
	}
}

void ACOMultiplayerCharacter::TriggerSlide()
{
	if (CurrentStance != ECovertOpsStance::Standing || !bIsSprinting)
	{
		return;
	}

	if (HasAuthority())
	{
		CurrentStance = ECovertOpsStance::Sliding;
		bIsSprinting = false;
		UpdateMovementState();
		GetWorldTimerManager().SetTimer(SlideTransitionTimer, this, &ACOMultiplayerCharacter::CompleteSlide, 0.5f, false);
	}
	else
	{
		ServerSetStance(ECovertOpsStance::Sliding);
	}
}

void ACOMultiplayerCharacter::ToggleProne()
{
	const ECovertOpsStance TargetStance = (CurrentStance == ECovertOpsStance::Prone) ? ECovertOpsStance::Standing : ECovertOpsStance::Prone;
	if (HasAuthority())
	{
		CurrentStance = TargetStance;
		UpdateMovementState();
	}
	else
	{
		ServerSetStance(TargetStance);
	}
}

void ACOMultiplayerCharacter::CompleteSlide()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentStance = ECovertOpsStance::Prone;
	UpdateMovementState();
}

void ACOMultiplayerCharacter::UpdateMovementState()
{
	if (!GetCharacterMovement())
	{
		return;
	}

	float DesiredSpeed = WalkSpeed;

	switch (CurrentStance)
	{
	case ECovertOpsStance::Crouched:
		DesiredSpeed = CrouchSpeed;
		Crouch();
		break;
	case ECovertOpsStance::Prone:
		DesiredSpeed = ProneSpeed;
		Crouch();
		break;
	case ECovertOpsStance::Sliding:
		DesiredSpeed = SlideSpeed;
		Crouch();
		break;
	case ECovertOpsStance::Standing:
	default:
		UnCrouch();
		DesiredSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = DesiredSpeed;
}

void ACOMultiplayerCharacter::SetCarriedFlag(ACOFlagObjectiveActor* InFlag)
{
	if (!HasAuthority())
	{
		return;
	}

	CarriedFlag = InFlag;
}

bool ACOMultiplayerCharacter::WasDamagedRecently(float WithinSeconds) const
{
	if (!GetWorld())
	{
		return false;
	}

	return (GetWorld()->GetTimeSeconds() - LastDamageTimeSeconds) <= FMath::Max(0.0f, WithinSeconds);
}
