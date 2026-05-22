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

## Current Audio Path Mode

```text
stub/test generator only
```

## Spatial Root Dependency

Spatial Root is checked out inside the plugin at:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

Current Spatial Root realtime output is driven by AlloLib `AudioIO`. That is not the same as routing into Unreal's mixer. A submix or audio bus can route samples after they enter Unreal, but it cannot act as a hardware output device selected by Spatial Root.

## Next Test

1. Build the project.
2. Add `USpatialRootTestToneComponent` to an actor or simple Blueprint.
3. Start the component and confirm audible generated audio from Unreal.
4. Document Unreal sample rate and output channel count if accessible.
