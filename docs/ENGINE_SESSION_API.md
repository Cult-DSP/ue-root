# EngineSession API Notes

## Source Files

- Public header: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/realtimeEngine/src/EngineSession.hpp`
- Implementation: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/realtimeEngine/src/EngineSession.cpp`
- Core CMake target: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/realtimeEngine/CMakeLists.txt`
- Canonical repo and branch: `https://github.com/Cult-DSP/spatialroot/tree/host-render`
- Current in-repo pin: `4e04d37`

## Construction

`EngineSession` is a default-constructible C++ class. The current Spatial Root source builds it into the `EngineSessionCore` static library target.

The ImGui app and realtime CLI both instantiate `EngineSession` directly. This supports external C++ host construction in principle, pending Unreal build/link work.

## Configuration Sequence

The verified lifecycle is:

```text
configureEngine(const EngineOptions&)
loadScene(const SceneInput&)
applyLayout(const LayoutInput&)
configureRuntime(const RuntimeParams&)
start()
update()
shutdown()
```

`shutdown()` is terminal for practical restart purposes. Construct a new session for a fresh run.

## ADM/BW64 Input

`SceneInput` contains:

```cpp
std::string scenePath;
std::string sourcesFolder;
std::string admFile;
```

`EngineSession::loadScene()` always loads a LUSID scene JSON from `scenePath`. If `admFile` is non-empty, it then calls `Streaming::loadSceneFromADM(*mSceneData, mConfig.admFile)`.

Current finding: direct ADM/BW64 audio streaming exists, but it still requires a LUSID scene file to define the source scene. ADM/BW64 alone is not enough through the current `EngineSession::loadScene()` implementation.

## Layout Input

`LayoutInput` contains:

```cpp
std::string layoutPath;
std::string remapCsvPath;
```

`EngineSession::applyLayout()` loads the layout JSON with `LayoutLoader::loadLayout()`. The layout defines speaker/subwoofer data and determines `mConfig.outputChannels`.

CSV remap is present but deprecated. Layout-derived routing is the intended path.

## Runtime Parameters

`RuntimeParams` contains:

```cpp
float masterGain = 0.5f;
float dbapFocus = 1.5f;
float speakerMixDb = 0.0f;
float subMixDb = 0.0f;
```

Runtime setters exist:

- `setPaused(bool)`
- `setMasterGain(float)`
- `setDbapFocus(float)`
- `setSpeakerMixDb(float)`
- `setSubMixDb(float)`
- `setElevationMode(ElevationMode)`

`dbapFocus` is clamped to a minimum of `0.1f`.

## Playback and Transport

`EngineSession::start()` creates the optional OSC parameter server, creates `RealtimeBackend`, wires `Streaming`, `Pose`, and `Spatializer`, starts the streaming loader, and starts the backend audio stream.

Pause/resume is controlled through `setPaused(bool)`.

Stop is effectively `shutdown()` for the current API; restart should create a new `EngineSession`.

## Audio Output Modes

The current realtime path is device-owned:

```text
EngineSession::start()
RealtimeBackend::init()
al::AudioIO::open()
al::AudioIO::start()
RealtimeBackend::processBlock(al::AudioIOData& io)
Spatializer::renderBlock(io, ...)
hardware output
```

An Internal Host Bus path is now available:

```text
setAudioOutputMode(AudioOutputMode::InternalHostBus)
prepareInternalHostBus(HostBusConfig)
renderHostBlock(interleaved buffer, frames, channels)
```

## Render-Buffer Availability

`EngineSession` now exposes a host-pull render-buffer method for interleaved output:

```cpp
int renderHostBlock(float* interleavedOutput, int numFrames, int numChannels);
```

The underlying render path remains `Spatializer::renderBlock(al::AudioIOData& io, ...)`, called from `RealtimeBackend::processBlock()`. For Internal Host Bus mode, `EngineSession::renderHostBlock()` invokes the same pipeline and copies the **internal render bus** into the host-provided interleaved buffer.

## Unreal Mixer Routing Status

Unreal mixer routing now has a callable host-owned PCM path via:

```cpp
int renderHostBlock(float* InterleavedOutput, int32 NumFrames, int32 NumChannels);
```

This path does **not** open a hardware device and can feed `USpatialRootRenderBusComponent` (or another UE procedural source). Runtime validation inside the Unreal editor is still pending.

## In-Repo Checkout

The current project dependency source is:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

`SpatialRootHost` links against the in-repo `EngineSessionCore` build artifact. The Unreal bridge includes `EngineSession.hpp` only in its `.cpp` implementation and treats the Spatial Root API as locked.

Current hardware-device fallback call sequence:

```text
USpatialRootBridge::Start()
configureEngine(oscPort = 0, 48 kHz, 512 frames)
loadScene(scenePath + admFile)
applyLayout(layoutPath)
configureRuntime(master gain, DBAP focus, speaker mix, sub mix)
start()
```

Important: `EngineSession::start()` still constructs `RealtimeBackend`, which opens AlloLib `AudioIO`. This proves Unreal can drive the `EngineSessionCore` lifecycle, but it is not Unreal mixer-native Spatial Root playback.

Current Unreal-owned host-bus call sequence:

```text
USpatialRootBridge::StartHostBus()
configureEngine(oscPort = 0, host sample rate, host frames)
loadScene(scenePath + admFile)
applyLayout(layoutPath)
configureRuntime(master gain, DBAP focus, speaker mix, sub mix)
setAudioOutputMode(AudioOutputMode::InternalHostBus)
prepareInternalHostBus(host frames + channels)
USpatialRootRenderBusComponent::OnGenerateAudio()
renderHostBlock(interleaved output, frames, channels)
```

Important: `StartHostBus()` does not call `EngineSession::start()`. Internal Host Bus mode is the preferred Unreal-owned output path because it lets Unreal own the hardware device.

## TransLab Benchmark Layout

The benchmark layout is:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/speaker_layouts/translab-sono-layout.json
```

The layout has 16 speakers plus 2 subwoofers, channels `0` through `17`, for an 18-channel physical output bus. The bridge parses this layout in Unreal and reports:

- Spatial Root required output channel count: 18
- Spatial Root internal render bus channel count: 18
- Unreal render bus channel count: 18

## Open Questions

- Which LUSID scene should pair with each ADM/BW64 file for the first Unreal test?
- Which LUSID scene should pair with each ADM/BW64 file for the first Unreal test?
- Can `USpatialRootRenderBusComponent` be auditioned in the editor with `bUseSpatialRootHostBus` enabled?
