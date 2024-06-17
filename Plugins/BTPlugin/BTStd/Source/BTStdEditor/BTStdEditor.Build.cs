using UnrealBuildTool;

public class BTStdEditor : ModuleRules
{
    public BTStdEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
        );
				
		
        PrivateIncludePaths.AddRange(
            new string[] {
                "BTStd/Private",
                "BTStdEditor/Private",
            }
        );
			
		
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "SceneOutliner",
            }
        );
			
		
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Projects",
                "SlateCore",
                "Slate",
                "DeveloperSettings",
                "InputCore",
                "EditorSubsystem",
                "BTCore",
                "BTStd",
				"PropertyEditor", 
                // ... add private dependencies that you statically link with here ...	
            }
        );

        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PrivateDependencyModuleNames.Add("EditorStyle");
            PrivateDependencyModuleNames.Add("SlateCore");
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
        );        
    }
}