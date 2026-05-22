# ue-root

`ue-root` is an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

The v0.1 target is intentionally narrow:

- macOS only.
- Local-machine-only development.
- Unreal Engine expected at `/Users/lucian/UE_5.7`.
- Spatial Root checked out inside this repository at `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`.
- ADM/BW64 input plus Spatial Root layout JSON.
- No visualization.
- No cross-platform packaging.

The first goal is to prove whether Unreal can host Spatial Root as an embedded spatial audio runtime, load ADM/BW64 content, load a layout, expose runtime controls, and route rendered audio through Unreal.

Example ADM/Atmos content is available at:

https://huggingface.co/datasets/lucianparisi/atmos-data/tree/main

## Current State

This repository currently contains a minimal Unreal project shell and a `SpatialRootHost` runtime plugin.

The plugin has:

- A minimal C++ module.
- A `USpatialRootSubsystem` Blueprint-facing control surface.
- A `USpatialRootBridge` that links and calls Spatial Root `EngineSessionCore`.
- A `FSpatialRootDiagnostics` model.
- A `USpatialRootTestToneComponent` Unreal synth source for proving generated audio can enter Unreal independently of Spatial Root.
- A `USpatialRootRenderBusComponent` 18-channel Unreal synth source for the intended internal render-bus handoff.

Spatial Root is checked out inside this repository at:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

That checkout is currently on `devel` at commit `b786ef8`, with submodules initialized recursively.

The realtime engine has been built from that in-repo checkout. Key local artifacts:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/build/spatial_engine/realtimeEngine/libEngineSessionCore.a
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/build/spatial_engine/realtimeEngine/spatialroot_realtime
```

## Open in Unreal

Preferred editor path:

```text
/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app
```

Project file:

```text
Unreal/UERoot.uproject
```

Generate project files:

```bash
"/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" -project="/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject" -game
```

Build:

```bash
"/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh" UERootEditor Mac Development -project="/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject" -WaitMutex
```

## TransLab Benchmark

The first benchmark layout is:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/speaker_layouts/translab-sono-layout.json
```

It contains 16 speakers plus 2 subwoofers, using channels `0` through `17`. The bridge reports this as an 18-channel Spatial Root output requirement and configures the Unreal render-bus component for the same channel count.

## Audio Routing Status

Unreal-side generated audio is represented by a test-tone synth component.

Spatial Root `EngineSessionCore` is linked into the Unreal plugin and can be driven through the locked API. Current investigation found that Spatial Root realtime rendering is performed inside `RealtimeBackend::processBlock(al::AudioIOData& io)`, reached from an AlloLib `AudioIO` callback opened by `EngineSession::start()`. No current public `EngineSession` host-pull method was found for rendering PCM blocks into a caller-provided Unreal buffer.

The desired Unreal route is now represented in code as:

```text
Spatial Root layout channel N
Unreal render bus channel N
Unreal source/submix/master
Unreal-selected output device channel N
```

That path is not receiving Spatial Root PCM yet. When `EngineSession::start()` is used today, the honest audio-path mode is still `Spatial Root owns device`.
