// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BTCore : ModuleRules
{
	public BTCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"BTCore/Private",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "HTTP",
                "Projects",
				"SlateCore",
                "Serialization",
				"DeveloperSettings",
				"UMG",
				"InputCore",
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

		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PublicDefinitions.Add("BT_DEVTOOL_IS_ENABLED=1");
			PrivateDependencyModuleNames.Add("Networking");
            PrivateDependencyModuleNames.Add("Sockets");
            PrivateDependencyModuleNames.Add("WebSockets");
            PrivateDependencyModuleNames.Add("Json");
		}
		else
		{
			PublicDefinitions.Add("BT_DEVTOOL_IS_ENABLED=0");
		}
	}
}
