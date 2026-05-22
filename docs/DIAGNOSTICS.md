# Diagnostics

`FSpatialRootDiagnostics` is the first-pass diagnostic model exposed by the `SpatialRootHost` plugin.

## Fields

- `bEngineInitialized`
- `AdmPath`
- `LayoutPath`
- `bAdmLoaded`
- `bLayoutLoaded`
- `TransportState`
- `UnrealSampleRate`
- `UnrealOutputChannelCount`
- `SpatialRootRequiredOutputChannelCount`
- `RenderedChannelCount`
- `UnderrunCount`
- `LastError`
- `LastOperation`
- `AudioPathMode`

## Audio Path Modes

- `Unknown`
- `UEProceduralSource`
- `SpatialRootOwnsDevice`
- `StubTestGeneratorOnly`
- `FailedNotConnected`

## Current Gaps

Unreal sample rate and output channel count are not populated yet.

Spatial Root required output channel count and rendered channel count are not populated until `EngineSessionCore` is linked and the layout path is available from the bridge.

