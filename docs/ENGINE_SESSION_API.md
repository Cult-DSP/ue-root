# EngineSession API Notes

## Source Files

- Public header: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.hpp`
- Implementation: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.cpp`
- Core CMake target: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/CMakeLists.txt`

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

## Render-Buffer Availability

No public host-pull render-buffer method was found on `EngineSession`.

The rendering function found is `Spatializer::renderBlock(al::AudioIOData& io, ...)`, called from `RealtimeBackend::processBlock()`. Its output target is AlloLib `AudioIOData`, owned by the AlloLib audio callback.

## Missing API for Unreal Mixer Routing

Unreal mixer routing needs a callable API shaped like:

```cpp
bool RenderNextBlock(float* InterleavedOutput, int32 NumFrames, int32 NumChannels);
```

or an equivalent planar-buffer API, where Unreal owns the destination buffer and Spatial Root does not open a hardware device.

Current exact blocker:

```text
No current host-pull render-buffer path found.
Unreal mixer routing requires a future Spatial Root render-buffer API or a deliberate wrapper around the existing spatializer/streaming internals.
```

## In-Repo Checkout

The current project dependency source is:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot
```

It was cloned fresh from `/Users/lucian/projects/spatialroot`, checked out on `devel`, and initialized with recursive submodules.

## Open Questions

- Which LUSID scene should pair with each ADM/BW64 file for the first Unreal test?
- Can `EngineSessionCore` and its AlloLib/libsndfile dependencies be cleanly linked by Unreal Build Tool without vendoring or install/export changes?
- Should the in-repo Spatial Root checkout be tracked as a git submodule by `ue-root`, or remain a local nested clone used for development builds?
