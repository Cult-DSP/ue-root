using UnrealBuildTool;

public class SpatialRootHost : ModuleRules
{
    public SpatialRootHost(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "AudioMixer"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Projects"
        });
    }
}

