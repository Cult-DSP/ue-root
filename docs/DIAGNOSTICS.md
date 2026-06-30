# Diagnostics

`FSpatialRootDiagnostics` is the first-pass diagnostic model exposed by the `SpatialRootHost` plugin.

## Fields

- `bEngineInitialized`
- `AdmPath`
- `LusidScenePath`
- `LayoutPath`
- `bAdmLoaded`
- `bLusidSceneLoaded`
- `bLayoutLoaded`
- `TransportState`
- `UnrealSampleRate`
- `UnrealOutputChannelCount`
- `SpatialRootRequiredOutputChannelCount`
- `SpatialRootInternalRenderBusChannelCount`
- `RenderedChannelCount`
- `UnrealRenderBusChannelCount`
- `UnderrunCount`
- `LastError`
- `LastWarning`
- `LastOperation`
- `AudioPathMode`

## Audio Path Modes

- `Unknown`
- `UEProceduralSource`
- `SpatialRootOwnsDevice`
- `StubTestGeneratorOnly`
- `FailedNotConnected`

## Current Status

The bridge now parses the TransLab layout JSON and reports its 18-channel requirement before starting Spatial Root.

`USpatialRootRenderBusComponent` reports its initialized sample rate and configured channel count to the bridge through `SetUnrealAudioFormat()`. This still needs editor/runtime verification against the actual selected output device.

Spatial Root status polling currently captures underruns from `EngineSession::queryStatus()` after lifecycle actions. A regular diagnostics tick still needs to be added.

`UUERootControlPanel` displays the current diagnostics snapshot every tick while the runtime UI is visible.
