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

Unreal sample rate and output channel count are not populated yet.

Spatial Root status polling currently captures underruns from `EngineSession::queryStatus()` after lifecycle actions. A regular diagnostics tick still needs to be added.
