// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hexshot : ModuleRules
{
	public Hexshot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		PublicDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "RawInput", "InputDevice", "ApplicationCore" });
	}
}
