using UnrealBuildTool;

public class BTPersonaViewEditor : ModuleRules
{
	public BTPersonaViewEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.AddRange(
			new string[]
			{
				System.IO.Path.Combine(GetModuleDirectory("SceneOutliner"), "Private"), // ComponentWithSockets を使用するために必要
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "UnrealEd",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore", 
				"Persona",
				"PropertyEditor", 
				"DeveloperSettings",
				"SceneOutliner",
				"EditorSubsystem",
				"BTPersonaView",
				"BTStd",
			}
		);
	}
}