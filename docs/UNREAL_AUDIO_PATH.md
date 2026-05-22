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

The plugin also includes `USpatialRootRenderBusComponent`, an 18-channel `USynthComponent` queue intended to act as the Unreal-side render-bus insertion point. It accepts interleaved float buffers through C++ and outputs queued samples through Unreal's generated-audio source path. When the queue is empty it outputs silence and increments an underrun counter.

For the TransLab benchmark, Spatial Root layout channels map directly to Unreal render-bus channels:

```text
Spatial Root layout channel/deviceChannel N
Unreal render bus channel N
Unreal source/submix/master output
hardware device channel N, if Unreal exposes the channel
```

This mapping is only meaningful once Spatial Root can provide rendered PCM to the component. Current `EngineSessionCore` does not expose a public host-pull render call.

## Current Audio Path Mode

```text
UE procedural source, for the Unreal render bus component
Spatial Root owns device, when EngineSession::start() is used
```

## Spatial Root Dependency

Spatial Root is checked out inside the plugin at:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

Current Spatial Root realtime output is driven by AlloLib `AudioIO`. That is not the same as routing into Unreal's mixer. A submix or audio bus can route samples after they enter Unreal, but it cannot act as a hardware output device selected by Spatial Root.

## Next Test

1. Build the project.
2. Add `USpatialRootRenderBusComponent` to an actor or simple Blueprint and configure it for 18 channels.
3. Add `USpatialRootTestToneComponent` separately for an audible basic source sanity check.
4. Start the components and confirm Unreal sample rate/output channel count if accessible.
5. Start `EngineSession` with a LUSID scene, ADM file, and the TransLab layout.
6. Document whether `EngineSession::start()` can run inside Unreal and which device/channel-count errors occur.
