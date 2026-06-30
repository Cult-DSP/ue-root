# Unreal Audio Path Notes

## Target Path

```text
Spatial Root rendered PCM
Unreal procedural source or synth generator
Unreal submix/master
Unreal output device
```

## Current Implementation

The `SpatialRootHost` plugin includes `USpatialRootTestToneComponent`, a `USynthComponent`-based generated audio source. It is intended to verify that plugin-generated audio can enter Unreal's audio graph before Spatial Root-rendered samples are attempted.

The plugin also includes `USpatialRootRenderBusComponent`, an 18-channel `USynthComponent` queue intended to act as the Unreal-side render-bus insertion point. It accepts interleaved float buffers through C++ and outputs queued samples through Unreal's generated-audio source path. When the queue is empty it outputs silence and increments an underrun counter. The component can also pull directly from the new Internal Host Bus API when `bUseSpatialRootHostBus` is enabled and a bridge reference is provided.

`ASpatialRootHostTestActor` now provides a minimal source-controlled runtime harness. It owns a `USpatialRootTestToneComponent` and `USpatialRootRenderBusComponent`, obtains the `USpatialRootBridge` from `USpatialRootSubsystem`, assigns that bridge to the render bus, and exposes Blueprint-callable methods to start/stop either the test tone or the Spatial Root Internal Host Bus path.

For the TransLab benchmark, Spatial Root layout channels map directly to Unreal render-bus channels:

```text
Spatial Root layout channel/deviceChannel N
Unreal render bus channel N
Unreal source/submix/master output
hardware device channel N, if Unreal exposes the channel
```

This mapping is only meaningful once Spatial Root can provide rendered PCM to the component. `EngineSessionCore` now exposes `renderHostBlock()` for host-pull rendering, and `USpatialRootRenderBusComponent` can invoke it through `USpatialRootBridge` when configured.

## Current Audio Path Mode

```text
UE procedural source, for the Unreal render bus component
Spatial Root owns device, when EngineSession::start() is used
UE procedural source + Internal Host Bus, when `bUseSpatialRootHostBus` is enabled and a bridge reference is set
```

## Spatial Root Dependency

Spatial Root is checked out inside the plugin at:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

This submodule points to the canonical Spatial Root repository on the `host-render` branch:

```text
https://github.com/Cult-DSP/spatialroot/tree/host-render
```

Current Spatial Root realtime output is driven by AlloLib `AudioIO` in HardwareDevice mode. Internal Host Bus mode keeps rendering in-process while allowing Unreal to own the final output device.

## Next Test

1. Build the project.
2. Create a minimal test map and place `ASpatialRootHostTestActor`.
3. Set `AdmBwfPath`, `LusidScenePath`, `LayoutPath`, `RequestedChannelCount`, `HostBusBlockSize`, and `HostBusSampleRate` on the actor.
4. Call `StartTestTone()` and confirm Unreal-generated audio exits the selected output device.
5. Call `StartSpatialRootHostBus()` and confirm `renderHostBlock()` feeds `USpatialRootRenderBusComponent`.
6. Verify rendered audio exits Unreal's device at the expected channels and document any channel-count or sample-rate mismatches.

The Internal Host Bus API (`renderHostBlock()`, `prepareInternalHostBus()`, `setAudioOutputMode()`) is implemented and documented in `SpatialRoot/spatialroot/internalDocs/HOST_RENDER_BACKEND.md`. Runtime audio validation in the Unreal editor is still pending.
