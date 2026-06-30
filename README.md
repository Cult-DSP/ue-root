# ue-root

`ue-root` is an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

The v0.1 target is intentionally narrow:

- macOS only.
- Local-machine-only development.
- Unreal Engine expected at `/Users/lucian/UE_5.7`.
- Spatial Root checked out inside this repository at `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`.
- Canonical Spatial Root source: `https://github.com/Cult-DSP/spatialroot/tree/host-render`.
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
- An `ASpatialRootHostTestActor` runtime harness that owns the test tone and render-bus synth components and exposes Blueprint-callable start/stop methods for editor validation.
- A native C++ `UUERootControlPanel` that builds a basic UMG control panel at runtime.
- An `AUERootGameMode` configured as the default game mode to spawn the harness and panel into the default map.

Spatial Root is checked out inside this repository at:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

That checkout is pinned to the canonical `host-render` branch at commit `4e04d37`, with submodules initialized recursively. The older `spatialroot-embedding` fork is no longer the canonical dependency for this project.

The realtime engine has been built from that in-repo checkout. Key local artifacts:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/build/source/spatial_engine/realtimeEngine/libEngineSessionCore.a
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/build/source/spatial_engine/realtimeEngine/spatialroot_realtime
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
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/speaker_layouts/translab-sono-layout.json
```

It contains 16 speakers plus 2 subwoofers, using channels `0` through `17`. The bridge reports this as an 18-channel Spatial Root output requirement and configures the Unreal render-bus component for the same channel count.

## Audio Routing Status

Unreal-side generated audio is represented by a test-tone synth component.

Spatial Root `EngineSessionCore` is linked into the Unreal plugin and can be driven through the locked API. The realtime render path still lives inside `RealtimeBackend::processBlock(al::AudioIOData& io)`, but an Internal Host Bus API now exists to render interleaved PCM blocks into Unreal-owned buffers without opening a hardware device.

The desired Unreal route is now represented in code as:

```text
Spatial Root layout channel N
Unreal render bus channel N
Unreal source/submix/master
Unreal-selected output device channel N
```

That path is now available through the host-bus API; the Unreal bridge can invoke `renderHostBlock()` when `bUseSpatialRootHostBus` is enabled, but runtime verification is still pending.

## Runtime Test Harness

The default game mode spawns `ASpatialRootHostTestActor` and a native `UUERootControlPanel` when the project runs. The panel provides editable ADM/BW64, LUSID scene, and layout paths, transport buttons, runtime sliders, a live update toggle, an Apply Params button, and diagnostics.

The harness exposes:

- `StartTestTone()` / `StopTestTone()` for a basic Unreal synth sanity check.
- `StartSpatialRootHostBus()` / `StopSpatialRootHostBus()` for the Internal Host Bus path.
- `GetDiagnostics()` for the current bridge status.

The initial content folders are present at `Unreal/Content/UI` and `Unreal/Content/Maps`, but no binary `.uasset` map or widget has been authored. The current UI is source-controlled C++.

Default candidate test values are:

- ADM/BW64 candidate: `/Users/lucian/projects/spatialroot/sourceData/CANYON-ATMOS-LFE.wav`
- LUSID scene candidate: `/Users/lucian/projects/spatialroot/sourceData/lusid_package/scene.lusid.json`
- Layout: the in-repo TransLab layout

These are candidates, not a confirmed-good pair, until an interactive editor run verifies loading and audible output.
