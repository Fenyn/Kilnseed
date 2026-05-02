using UnrealBuildTool;

public class Kilnseed : ModuleRules
{
	public Kilnseed(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"UMG",
			"Slate",
			"SlateCore",
			"CommonUI",
			"SmartObjectsModule",
			"StateTreeModule",
			"PCG",
			"AIModule",
			"NavigationSystem",
			"Niagara"
		});
	}
}
