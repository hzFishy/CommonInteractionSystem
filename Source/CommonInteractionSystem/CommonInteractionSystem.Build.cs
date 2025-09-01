// By hzFishy - 2025 - Do whatever you want with it.

using UnrealBuildTool;

public class CommonInteractionSystem : ModuleRules
{
	public CommonInteractionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				// Core
				"Core", "CoreUObject",
				// GAS stuff
				"GameplayTasks", "GameplayTags", "GameplayAbilities",
				// UI
				"UMG",
				// Fishy Deps
				"FishyUtils", "FishyWorldSpaceWidgets",
				// Miscs
				"DeveloperSettings"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// Core
				"Engine",
				// UI
				"Slate", "SlateCore", 
			}
		);
	}
}
