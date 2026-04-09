// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer/Core/COMultiplayerGameInstance.h"
#include "CovertOps1.h"
#include "OnlineSubsystem.h"

void UCOMultiplayerGameInstance::Init()
{
	Super::Init();

	const IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	UE_LOG(LogCovertOps1, Log, TEXT("Multiplayer GameInstance initialized. Online subsystem: %s"),
		OSS ? *OSS->GetSubsystemName().ToString() : TEXT("None"));
}

void UCOMultiplayerGameInstance::Shutdown()
{
	UE_LOG(LogCovertOps1, Log, TEXT("Multiplayer GameInstance shutdown."));
	Super::Shutdown();
}
