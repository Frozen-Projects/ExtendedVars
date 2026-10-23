using UnrealBuildTool;

public class ExtendedVars : ModuleRules
{
	public ExtendedVars(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "MediaAssets",
            });
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "RHI",
				"RenderCore",
                "ImageWrapper",
                "ApplicationCore",
                "Networking",
				"Sockets",
                "Json",
                "JsonUtilities",
                "JsonBlueprintUtilities",
                "UMG",
                "Slate",
                "SlateCore",
            });
	}
}