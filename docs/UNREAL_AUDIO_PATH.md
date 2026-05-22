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
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/spatialroot-embedding
```

Current Spatial Root realtime output is driven by AlloLib `AudioIO` in HardwareDevice mode. Internal Host Bus mode keeps rendering in-process while allowing Unreal to own the final output device.

## Next Test

1. Build the project.
2. Add `USpatialRootRenderBusComponent` to an actor or simple Blueprint and configure it for 18 channels.
3. Add `USpatialRootTestToneComponent` separately for an audible basic source sanity check.
4. Start the components and confirm Unreal sample rate/output channel count if accessible.
5. Start `EngineSession` with a LUSID scene, ADM file, and the TransLab layout using `AudioOutputMode::InternalHostBus` and `prepareInternalHostBus()`.
6. Confirm `renderHostBlock()` is feeding the render bus component via `USpatialRootBridge` with `bUseSpatialRootHostBus` enabled.
7. Verify rendered audio exits Unreal's device at the expected channels and document any channel-count or sample-rate mismatches.

The Internal Host Bus API (`renderHostBlock()`, `prepareInternalHostBus()`, `setAudioOutputMode()`) is implemented and documented in `spatialroot-embedding/internalDocs/HOST_RENDER_BACKEND.md`.
