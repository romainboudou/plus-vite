// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Plus_Vite : ModuleRules
{
	public Plus_Vite(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
