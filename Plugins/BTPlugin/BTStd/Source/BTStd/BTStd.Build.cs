// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BTStd : ModuleRules
{
	public BTStd(ReadOnlyTargetRules Target) : base(Target)
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
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Niagara",
                "NiagaraCore",
                "NiagaraAnimNotifies",
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
				"BTCore"
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
            PrivateDependencyModuleNames.Add("Networking");
            PrivateDependencyModuleNames.Add("Sockets");
            PrivateDependencyModuleNames.Add("WebSockets");
            PrivateDependencyModuleNames.Add("Json");
        }
    }
}
