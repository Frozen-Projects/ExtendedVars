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
                "RHI",				// FRHICommandListImmediate and Enqueue Rendering Commands
				"RenderCore",		// FRHICommandListImmediate and Enqueue Rendering Commands
				"UMG",
                "MediaAssets",
                "ImageWrapper",
            }
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
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