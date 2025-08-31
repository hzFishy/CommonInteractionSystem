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
				"Core", "CoreUObject", "FishyUtils", "GameplayTasks", "GameplayTasks", "GameplayAbilities"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}
