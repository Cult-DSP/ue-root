# Task Log

## Current State

The repository now has a minimal Unreal project shell at `Unreal/UERoot.uproject` and a `SpatialRootHost` runtime plugin skeleton. The plugin exposes a bridge, subsystem, diagnostics model, and Unreal test-tone synth component. A fresh Spatial Root clone exists inside the plugin ThirdParty tree, on `devel` at `b786ef8`, with recursive submodules initialized. The Unreal shell/plugin builds, and Spatial Root's realtime engine builds inside the repo. Spatial Root is not linked into Unreal yet.

## Confirmed Facts

- Fact: The initial repo had only `AGENTS.md`, `DESIGN.md`, `UE_ROOT_AGENT_SKILL.md`, and `SPATIAL_ROOT_AGENT_SKILL.md`.
- Evidence: Repository tree inspection.
- File or command: `rg --files`

- Fact: `AGENTS.md` expected the two skill docs under `docs/`, but they were present at the repo root.
- Evidence: `sed` failed for `docs/UE_ROOT_AGENT_SKILL.md` and `docs/SPATIAL_ROOT_AGENT_SKILL.md`; root-level files were readable.
- File or command: `sed -n '1,260p' docs/UE_ROOT_AGENT_SKILL.md`; `sed -n '1,280p' UE_ROOT_AGENT_SKILL.md`

- Fact: Preferred Unreal Engine path exists.
- Evidence: `/Users/lucian/UE_5.7` contains `Engine`, `FeaturePacks`, and `Templates`.
- File or command: `ls -la /Users/lucian/UE_5.7`

- Fact: Unreal editor binary and build scripts exist.
- Evidence: `UnrealEditor`, `UnrealEditor.app`, `Build.sh`, and `GenerateProjectFiles.sh` were found.
- File or command: `ls -la /Users/lucian/UE_5.7/Engine/Binaries/Mac`; `ls -la /Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac`

- Fact: Preferred Spatial Root repo exists.
- Evidence: `/Users/lucian/projects/spatialroot` contains `CMakeLists.txt`, `spatial_engine`, `gui`, `sourceData`, and related source directories.
- File or command: `ls -la /Users/lucian/projects/spatialroot`

- Fact: A fresh Spatial Root clone now exists inside this repo.
- Evidence: The clone is on branch `devel`, commit `b786ef8`, and has a clean worktree after recursive submodule initialization.
- File or command: `git clone --branch devel /Users/lucian/projects/spatialroot Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`; `git submodule update --init --recursive`; `git status --short`

- Fact: The in-repo Spatial Root checkout is now represented as a parent repo submodule, not vendored source.
- Evidence: Parent index records mode `160000` for the Spatial Root path and `git ls-files` lists only one path under `SpatialRoot/spatialroot`.
- File or command: `git ls-files --stage | rg "160000|SpatialRoot/spatialroot|\\.gitmodules"`; `git ls-files Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot | wc -l`

- Fact: Spatial Root checkout is on branch `devel` at commit `b786ef8`.
- Evidence: Git branch and revision commands.
- File or command: `git branch --show-current`; `git rev-parse --short HEAD`

- Fact: Spatial Root worktree is dirty before this session's edits.
- Evidence: Modified `LUSID`, modified `cult_transcoder`, and untracked `thirdparty/allolib/`.
- File or command: `git status --short`

- Fact: `EngineSession` is declared in Spatial Root and builds as `EngineSessionCore`.
- Evidence: Header and CMake inspection.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.hpp`; `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/CMakeLists.txt`

- Fact: Current `EngineSession::loadScene()` requires a LUSID scene path even when ADM direct streaming is used.
- Evidence: `loadScene()` calls `JSONLoader::loadLusidScene(mConfig.scenePath)` before checking `admFile` and calling `Streaming::loadSceneFromADM`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.cpp`

- Fact: Current Spatial Root realtime audio path opens an AlloLib audio device.
- Evidence: `EngineSession::start()` creates `RealtimeBackend`; `RealtimeBackend::init()` calls `mAudioIO.open()`; `RealtimeBackend::start()` calls `mAudioIO.start()`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/RealtimeBackend.hpp`

- Fact: No public host-pull render-buffer method was found on `EngineSession`.
- Evidence: `EngineSession.hpp` exposes lifecycle, setters, status, diagnostics, and shutdown, but no render/process block method that writes into a host-provided buffer.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.hpp`

- Fact: `UERootEditor` builds successfully with the `SpatialRootHost` plugin skeleton.
- Evidence: UnrealBuildTool completed 15 actions and linked `UnrealEditor-UERoot.dylib` and `UnrealEditor-SpatialRootHost.dylib`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: Spatial Root realtime engine builds successfully from the in-repo clone.
- Evidence: Build completed and produced `libEngineSessionCore.a` plus `spatialroot_realtime`.
- File or command: `./build.sh --engine-only`

## Open Questions

- Question: Should Spatial Root be integrated into `ue-root` as a git submodule, copied ThirdParty source, or linked from the local absolute path for the first pass?
- Why it matters: A nested fresh clone now exists, but the parent repo still needs an intentional tracking strategy.
- Suggested next step: Decide whether to convert the nested clone to a formal git submodule or leave it as a local development checkout.

- Question: Which local ADM/BW64 file and layout JSON should be the first canonical Unreal test pair?
- Why it matters: SourceData contains candidate WAV and LUSID files, and layout JSONs exist in the in-repo clone, but a known-good pair still needs confirmation.
- Suggested next step: Try a conservative pair such as a known ADM WAV, `sourceData/lusid_package/scene.lusid.json`, and `spatial_engine/speaker_layouts/stereo.json` only after validating the scene/source mapping.

- Question: Can Unreal 5.7's synth/procedural source report output channel count in the desired place?
- Why it matters: Diagnostics must distinguish requested, rendered, and actual output channel counts.
- Suggested next step: Build and run the test-tone component, then query audio device handles from Unreal.

## Blockers

- Blocker: Spatial Root-rendered PCM cannot currently be claimed to enter Unreal's mixer.
- Evidence: Current realtime path renders inside `RealtimeBackend::processBlock(al::AudioIOData& io)`, reached from AlloLib `AudioIO`, and no `EngineSession` host-pull render method was found.
- Possible workaround: Keep the Unreal test-tone generator and bridge skeleton; add a future Spatial Root render-buffer API or deliberate wrapper around current internals.

## Last Completed Tasks

- Task: Created minimal Unreal project shell.
- Result: Added `Unreal/UERoot.uproject`, targets, module, and basic config. Updated targets to `BuildSettingsVersion.V6` for Unreal 5.7.
- Files changed: `Unreal/UERoot.uproject`, `Unreal/Source/UERoot*`, `Unreal/Config/DefaultEngine.ini`

- Task: Created `SpatialRootHost` plugin skeleton.
- Result: Added plugin descriptor, module, bridge, subsystem, diagnostics, and test-tone synth component.
- Files changed: `Unreal/Plugins/SpatialRootHost/**`

- Task: Wrote first-pass investigation docs.
- Result: Documented EngineSession API findings, Unreal audio path status, diagnostics, example content, and task log.
- Files changed: `docs/ENGINE_SESSION_API.md`, `docs/UNREAL_AUDIO_PATH.md`, `docs/DIAGNOSTICS.md`, `docs/EXAMPLE_CONTENT.md`, `docs/TASK_LOG.md`

- Task: Drafted README and roadmap.
- Result: Added setup, scope, current state, and audio routing status.
- Files changed: `README.md`, `ROADMAP.md`

- Task: Cloned and built Spatial Root inside the repo.
- Result: Fresh in-repo clone on `devel` at `b786ef8`, recursive submodules initialized, realtime engine build succeeded.
- Files changed: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`

- Task: Formalized Spatial Root as a parent repo submodule.
- Result: Added `.gitmodules` mapping to `https://github.com/Cult-DSP/spatialroot.git` on `devel`; parent tracks only the gitlink at commit `b786ef8`, while submodule build output remains ignored inside the submodule.
- Files changed: `.gitmodules`, `docs/TASK_LOG.md`

- Task: Built Unreal shell/plugin.
- Result: `UERootEditor` build succeeded.
- Files changed: Generated Unreal build outputs under ignored directories.

## Local Unreal Setup

- Unreal path checked: `/Users/lucian/UE_5.7`
- Exists: Yes
- Editor launch path: `/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app`
- Project path: `/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject`
- Build command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`
- Build result: Succeeded.
- Plugin status: Skeleton created.
- Spatial Root dependency status: In-repo clone found and built; not linked into Unreal plugin yet.
- In-repo Spatial Root checkout: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, branch `devel`, commit `b786ef8`, recursive submodules initialized.
- Audio test status: Test-tone synth component added and compiled; not yet run/auditioned in editor.
- Next step: Add a minimal level/actor or Blueprint to instantiate the test tone and expose bridge diagnostics.

## Local Test Data

- sourceData path: `/Users/lucian/projects/spatialroot/sourceData`
- Exists: Yes
- ADM/BW64 candidates: `CANYON-ATMOS-LFE.wav`, `ASCENT-ATMOS-LFE.wav`, `OFFERING-ATMOS-LFE.wav`, `SWALE-ATMOS-LFE.wav`, `EDEN-ATMOS-MIX-LFE.wav`, `sphere-ascent-atmos.wav`, `360RA_test.wav`
- Layout JSON candidates: `spatial_engine/speaker_layouts/stereo.json`, `spatial_engine/speaker_layouts/example_layouts/5_1.json`, `spatial_engine/speaker_layouts/example_layouts/quad_4.json`, `spatial_engine/speaker_layouts/example_layouts/ring8_top4.json`, and additional layouts in the in-repo clone.
- LUSID candidates: `sourceData/lusid_package/scene.lusid.json`, `sourceData/qgoo_LUSID/scene.lusid.json`
- Selected first ADM/BW64 test file: None selected.
- Selected first layout file: None selected.
- Open questions: Need a confirmed ADM/BW64 plus LUSID scene plus layout JSON pair.

## UE Audio Investigation

- Audio entry point tested: Not runtime-tested yet; `USynthComponent` skeleton added.
- Test tone/silence status: Source code compiled successfully; runtime audio test pending.
- Submix routing status: Not attempted.
- UE sample rate: Unknown.
- UE output channel count: Unknown.
- Spatial Root render-buffer path: No public host-pull path found.
- Blocker: Spatial Root realtime path currently owns an AlloLib audio device.
- Next step: Build plugin and verify generated Unreal audio.

## Spatial Root Investigation

- Spatial Root path: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`
- Branch: `devel`
- Commit: `b786ef8`
- EngineSession found: Yes
- EngineSession source files: `spatial_engine/realtimeEngine/src/EngineSession.hpp`, `spatial_engine/realtimeEngine/src/EngineSession.cpp`
- ADM/BW64 load path: `SceneInput::admFile` with `Streaming::loadSceneFromADM`, after LUSID scene load.
- Layout load path: `LayoutInput::layoutPath` with `LayoutLoader::loadLayout`.
- Runtime params path: `RuntimeParams` and direct setters on `EngineSession`.
- Render-buffer path: No public `EngineSession` host-pull render-buffer path found.
- Device-owned fallback: `RealtimeBackend` opens and starts AlloLib `AudioIO`.
- Build/link status: `EngineSessionCore` built successfully in the in-repo clone; Unreal link not attempted.
- Main blocker: Need host-owned PCM render API or a deliberate wrapper before Unreal mixer routing can be real.
- Next recommended step: Build the Unreal shell, then decide Spatial Root dependency strategy.

## Next Recommended Tasks

1. Create a minimal level/actor or Blueprint binding for `USpatialRootTestToneComponent`.
2. Wire Unreal Build Tool to the in-repo `EngineSessionCore` artifact and include paths.
3. Implement the bridge lifecycle against `EngineSession` while keeping audio path mode honest.
4. Locate a known-good ADM/BW64 plus LUSID scene plus layout JSON test pair.
5. Keep the Spatial Root submodule pinned intentionally when updating the dependency.

## Do Not Repeat

- Failed attempt: Looking for skill docs under `docs/` before they exist there.
- Why it failed: The initial repository has `UE_ROOT_AGENT_SKILL.md` and `SPATIAL_ROOT_AGENT_SKILL.md` at the repo root.
- What to do instead: Read the root-level files until the repository structure is normalized.
