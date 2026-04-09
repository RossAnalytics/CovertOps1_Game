// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CovertOps1 : ModuleRules
{
	public CovertOps1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"NetCore",
			"HTTP",
			"Json",
			"JsonUtilities",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CovertOps1",
			"CovertOps1/Multiplayer",
			"CovertOps1/Multiplayer/Components",
			"CovertOps1/Multiplayer/Core",
			"CovertOps1/Multiplayer/Data",
			"CovertOps1/Multiplayer/GAS",
			"CovertOps1/Multiplayer/Interfaces",
			"CovertOps1/Multiplayer/Modes",
			"CovertOps1/Multiplayer/Objectives",
			"CovertOps1/Multiplayer/UI",
			"CovertOps1/Multiplayer/Visual",
			"CovertOps1/Multiplayer/Weapons",
			"CovertOps1/Variant_Horror",
			"CovertOps1/Variant_Horror/UI",
			"CovertOps1/Variant_Shooter",
			"CovertOps1/Variant_Shooter/AI",
			"CovertOps1/Variant_Shooter/UI",
			"CovertOps1/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
