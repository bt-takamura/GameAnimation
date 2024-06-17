using UnrealBuildTool;

public class SNPluginEditor : ModuleRules
{
	public SNPluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.AddRange(
			new string[]
			{
				"SNPlugin",
				"SNPluginEditor",
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetDefinition",
				"Core", 
				"UnrealEd", 
				"AnimGraph",
				"BlueprintGraph", 
				"ToolMenus",
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
				"SNPlugin",
			}
		);
	}
}