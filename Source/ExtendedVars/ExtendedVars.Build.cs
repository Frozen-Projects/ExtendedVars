// Some copyright should be here...

using System;
using System.IO;
using UnrealBuildTool;

public class ExtendedVars : ModuleRules
{
	public ExtendedVars(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        bUseUnity = false;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "MediaAssets",
            }
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "RHI",
				"RenderCore",
				"UMG",
                "ImageWrapper",
                "ApplicationCore",
                "Networking",
				"Sockets",
                "Json",
                "JsonUtilities",
                "JsonBlueprintUtilities",
            }
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}