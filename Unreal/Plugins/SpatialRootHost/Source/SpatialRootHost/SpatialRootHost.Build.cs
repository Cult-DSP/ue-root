using UnrealBuildTool;
using System.IO;

public class SpatialRootHost : ModuleRules
{
    public SpatialRootHost(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        string PluginRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../"));
        string SpatialRootRoot = Path.Combine(PluginRoot, "Source/ThirdParty/SpatialRoot/spatialroot");
        string SpatialRootBuild = Path.Combine(SpatialRootRoot, "build");
        string EngineSessionLib = Path.Combine(SpatialRootBuild, "spatial_engine/realtimeEngine/libEngineSessionCore.a");

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "AudioMixer",
            "Json"
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Projects"
        });

        PublicIncludePaths.AddRange(new[]
        {
            Path.Combine(SpatialRootRoot, "spatial_engine/realtimeEngine/src"),
            Path.Combine(SpatialRootRoot, "spatial_engine/src"),
            Path.Combine(SpatialRootRoot, "spatial_engine/src/renderer"),
            Path.Combine(SpatialRootRoot, "internal/cult-allolib/include")
        });

        if (File.Exists(EngineSessionLib))
        {
            PublicDefinitions.Add("SPATIALROOT_WITH_ENGINESESSION=1");
            PublicDefinitions.Add("AL_WINDOWS=0");

            PublicAdditionalLibraries.AddRange(new[]
            {
                EngineSessionLib,
                Path.Combine(SpatialRootBuild, "allolib-build/libal.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/Gamma/lib/libGamma.a"),
                Path.Combine(SpatialRootBuild, "libsndfile-build/libsndfile.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/rtaudio/librtaudio.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/rtmidi/librtmidi.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/liboscpack.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/libserial.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/glfw/src/libglfw3.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/glad/libglad.a"),
                Path.Combine(SpatialRootBuild, "allolib-build/external/libimgui.a")
            });

            PublicFrameworks.AddRange(new[]
            {
                "AudioToolbox",
                "AudioUnit",
                "Cocoa",
                "CoreAudio",
                "CoreFoundation",
                "CoreMIDI",
                "IOKit",
                "OpenGL"
            });
        }
        else
        {
            PublicDefinitions.Add("SPATIALROOT_WITH_ENGINESESSION=0");
        }
    }
}
