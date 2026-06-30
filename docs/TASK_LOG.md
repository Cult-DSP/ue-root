# Task Log

## Current State

The repository has a minimal Unreal project shell at `Unreal/UERoot.uproject` and a `SpatialRootHost` runtime plugin. Spatial Root is tracked as a submodule under `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, pointed at the canonical Cult-DSP `host-render` branch.

`SpatialRootHost` links `EngineSessionCore` from the Spatial Root submodule build artifact and compiles with real `EngineSession` lifecycle calls. The canonical Spatial Root `host-render` branch includes the Internal Host Bus API that renders PCM into a host-owned interleaved buffer without opening a device. The TransLab layout is the benchmark target, parsed as 18 output channels. `USpatialRootRenderBusComponent` can pull from the host bus via `USpatialRootBridge`, but this flow still needs in-editor runtime verification after the dependency repoint.

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

- Fact: The canonical Spatial Root host-render source is now `https://github.com/Cult-DSP/spatialroot/tree/host-render`.
- Evidence: The in-repo submodule was repointed to `https://github.com/Cult-DSP/spatialroot.git` with `branch = host-render`, then pinned to commit `4e04d37`.
- File or command: `.gitmodules`; `git rev-parse --short HEAD`

- Fact: A fresh Spatial Root clone now exists inside this repo.
- Evidence: The clone is on branch `devel`, commit `b786ef8`, and has a clean worktree after recursive submodule initialization.
- File or command: `git clone --branch devel /Users/lucian/projects/spatialroot Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`; `git submodule update --init --recursive`; `git status --short`

- Fact: The in-repo Spatial Root checkout is now represented as a parent repo submodule, not vendored source.
- Evidence: Parent index records mode `160000` for the Spatial Root path and `git ls-files` lists only one path under `SpatialRoot/spatialroot`.
- File or command: `git ls-files --stage | rg "160000|SpatialRoot/spatialroot|\\.gitmodules"`; `git ls-files Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot | wc -l`

- Fact: Spatial Root checkout is pinned detached at canonical `host-render` commit `4e04d37`.
- Evidence: Git branch and revision commands from the submodule.
- File or command: `git status --short --branch`; `git rev-parse --short HEAD`

- Fact: The preferred local Spatial Root checkout at `/Users/lucian/projects/spatialroot` was not edited during the dependency repoint.
- Evidence: It had pre-existing local dirty state on `devel`, while `origin/host-render` was used as the source ref for the submodule pin.
- File or command: `git status --short --branch`; `git branch -r --list 'origin/host-render'`

- Fact: `EngineSession` is declared in Spatial Root and builds as `EngineSessionCore`.
- Evidence: Header and CMake inspection.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.hpp`; `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/CMakeLists.txt`

- Fact: Current `EngineSession::loadScene()` requires a LUSID scene path even when ADM direct streaming is used.
- Evidence: `loadScene()` calls `JSONLoader::loadLusidScene(mConfig.scenePath)` before checking `admFile` and calling `Streaming::loadSceneFromADM`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/EngineSession.cpp`

- Fact: Current Spatial Root realtime audio path opens an AlloLib audio device.
- Evidence: `EngineSession::start()` creates `RealtimeBackend`; `RealtimeBackend::init()` calls `mAudioIO.open()`; `RealtimeBackend::start()` calls `mAudioIO.start()`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/realtimeEngine/src/RealtimeBackend.hpp`

- Fact: Internal Host Bus output mode was added to `EngineSession` for host-pull rendering.
- Evidence: `EngineSession` now exposes `setAudioOutputMode`, `prepareInternalHostBus`, and `renderHostBlock` with interleaved PCM output.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/realtimeEngine/src/EngineSession.hpp`

- Fact: `UERootEditor` builds successfully with the `SpatialRootHost` plugin skeleton.
- Evidence: UnrealBuildTool completed 15 actions and linked `UnrealEditor-UERoot.dylib` and `UnrealEditor-SpatialRootHost.dylib`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: Spatial Root realtime engine builds successfully from the in-repo clone.
- Evidence: Build completed and produced `libEngineSessionCore.a` plus `spatialroot_realtime`.
- File or command: `./build.sh --engine-only`

- Fact: `SpatialRootHost` links against `EngineSessionCore` without modifying the Spatial Root submodule.
- Evidence: `UERootEditor` build succeeded after adding Spatial Root static libraries and frameworks to `SpatialRootHost.Build.cs`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: The benchmark layout is TransLab, requiring 18 output channels.
- Evidence: `translab-sono-layout.json` contains 16 speakers and 2 subwoofers with channels `0` through `17`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/speaker_layouts/translab-sono-layout.json`

- Fact: A UE-side render-bus component exists, but it is not fed by Spatial Root PCM yet.
- Evidence: `USpatialRootRenderBusComponent` queues interleaved float blocks and outputs them through `USynthComponent`; no current `EngineSession` API provides those blocks.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/SpatialRootHost/Public/SpatialRootRenderBusComponent.h`

## Open Questions

- Question: Which local ADM/BW64 file and layout JSON should be the first canonical Unreal test pair?
- Why it matters: SourceData contains candidate WAV and LUSID files, and layout JSONs exist in the in-repo clone, but a known-good pair still needs confirmation.
- Suggested next step: Try a conservative pair such as a known ADM WAV, `sourceData/lusid_package/scene.lusid.json`, and `spatial_engine/speaker_layouts/stereo.json` only after validating the scene/source mapping.

- Question: Can Unreal 5.7's synth/procedural source report output channel count in the desired place?
- Why it matters: Diagnostics must distinguish requested, rendered, and actual output channel counts.
- Suggested next step: Build and run the test-tone component, then query audio device handles from Unreal.

## Blockers

- Blocker: Host-bus audio path still needs runtime validation in the Unreal editor.
- Evidence: `USpatialRootRenderBusComponent` now calls `renderHostBlock()` through `USpatialRootBridge`, but no in-editor run has been documented yet.
- Possible workaround: Run a minimal Blueprint with `bUseSpatialRootHostBus` enabled and confirm audio output plus diagnostics.

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

- Task: Repointed `ue-root` to canonical Spatial Root host-render.
- Result: Moved the submodule from the old `spatialroot-embedding` path to `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, updated `.gitmodules` to `https://github.com/Cult-DSP/spatialroot.git` on `host-render`, pinned the submodule to `4e04d37`, and rebuilt both Spatial Root engine-only and `UERootEditor` successfully.
- Files changed: `.gitmodules`, `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, `SpatialRootHost.Build.cs`, `SpatialRootBridge.cpp`, docs updates

- Task: Built Unreal shell/plugin.
- Result: `UERootEditor` build succeeded.
- Files changed: Generated Unreal build outputs under ignored directories.

- Task: Linked EngineSessionCore into the Unreal plugin.
- Result: `SpatialRootHost` compiles and links with the Spatial Root static libraries from the submodule build.
- Files changed: `SpatialRootHost.Build.cs`, `SpatialRootBridge.h`, `SpatialRootBridge.cpp`

- Task: Added Unreal internal render-bus source.
- Result: `USpatialRootRenderBusComponent` provides an 18-channel queued interleaved-float source for future Spatial Root PCM handoff.
- Files changed: `SpatialRootRenderBusComponent.h`, `SpatialRootRenderBusComponent.cpp`

- Task: Added Internal Host Bus host-pull API to Spatial Root.
- Result: `EngineSession` can render interleaved PCM blocks without opening a hardware device. Host bus configuration and warnings are documented.
- Files changed: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/realtimeEngine/src/EngineSession.*`, `RealtimeBackend.hpp`, `RealtimeTypes.hpp`, `Spatializer.hpp`, `Streaming.hpp`, `internalDocs/HOST_RENDER_BACKEND.md`

- Task: Wired Unreal render bus to host-pull PCM.
- Result: `USpatialRootRenderBusComponent` can call `renderHostBlock()` through `USpatialRootBridge` when `bUseSpatialRootHostBus` is enabled; diagnostics now capture last warnings.
- Files changed: `SpatialRootBridge.h`, `SpatialRootBridge.cpp`, `SpatialRootRenderBusComponent.h`, `SpatialRootRenderBusComponent.cpp`, `SpatialRootDiagnostics.h`, docs updates

- Task: Rebuilt `UERootEditor` after SpatialRoot/spatialroot path updates.
- Result: Unreal build succeeds with the updated include/library paths.
- Files changed: `SpatialRootHost.Build.cs`

## Local Unreal Setup

- Unreal path checked: `/Users/lucian/UE_5.7`
- Exists: Yes
- Editor launch path: `/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app`
- Project path: `/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject`
- Build command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`
- Build result: Succeeded after repointing to canonical `host-render`.
- Plugin status: Compiles with `EngineSessionCore` linked.
- Spatial Root dependency status: In-repo clone found and built; linked into the Unreal plugin.
- In-repo Spatial Root checkout: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, canonical `host-render`, commit `4e04d37`, recursive submodules initialized.
- Audio test status: Test-tone synth and render-bus synth components compile; not yet run/auditioned in editor.
- Next step: Add a minimal level/actor or Blueprint to instantiate the test tone and expose bridge diagnostics.

## Local Test Data

- sourceData path: `/Users/lucian/projects/spatialroot/sourceData`
- Exists: Yes
- ADM/BW64 candidates: `CANYON-ATMOS-LFE.wav`, `ASCENT-ATMOS-LFE.wav`, `OFFERING-ATMOS-LFE.wav`, `SWALE-ATMOS-LFE.wav`, `EDEN-ATMOS-MIX-LFE.wav`, `sphere-ascent-atmos.wav`, `360RA_test.wav`
- Layout JSON candidates: TransLab benchmark selected at `spatial_engine/speaker_layouts/translab-sono-layout.json`; other candidates include `stereo.json`, `example_layouts/5_1.json`, `example_layouts/quad_4.json`, and `example_layouts/ring8_top4.json`.
- LUSID candidates: `sourceData/lusid_package/scene.lusid.json`, `sourceData/qgoo_LUSID/scene.lusid.json`
- Selected first ADM/BW64 test file: None selected.
- Selected first layout file: `spatial_engine/speaker_layouts/translab-sono-layout.json`
- Open questions: Need a confirmed ADM/BW64 plus LUSID scene plus layout JSON pair.

## UE Audio Investigation

- Audio entry point tested: Not runtime-tested yet; `USynthComponent` skeleton added.
- Test tone/silence status: Test tone and 18-channel render-bus source code compiled successfully; runtime audio test pending.
- Submix routing status: Not attempted.
- UE sample rate: Unknown.
- UE output channel count: Unknown.
- Spatial Root render-buffer path: Internal Host Bus API now available.
- Spatial Root engine-only build: Succeeded from the canonical `host-render` submodule after clearing the old CMake cache from the previous submodule path.
- Blocker: Unreal runtime validation still pending (host bus path not auditioned in editor yet).
- Next step: Build plugin, enable host bus in `USpatialRootRenderBusComponent`, and verify `renderHostBlock()` output in-editor.

## Spatial Root Investigation

- Spatial Root path: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`
- Branch: canonical `host-render` pin, checked out detached in the submodule
- Commit: `4e04d37`
- EngineSession found: Yes
- EngineSession source files: `spatial_engine/realtimeEngine/src/EngineSession.hpp`, `spatial_engine/realtimeEngine/src/EngineSession.cpp`
- ADM/BW64 load path: `SceneInput::admFile` with `Streaming::loadSceneFromADM`, after LUSID scene load.
- Layout load path: `LayoutInput::layoutPath` with `LayoutLoader::loadLayout`.
- Runtime params path: `RuntimeParams` and direct setters on `EngineSession`.
- Render-buffer path: Internal Host Bus API now available via `EngineSession::renderHostBlock()`.
- Device-owned fallback: `RealtimeBackend` still opens and starts AlloLib `AudioIO` for HardwareDevice mode.
- Build/link status: `EngineSessionCore` built successfully in the in-repo clone and linked into `SpatialRootHost`.
- Main blocker: Unreal host-bus path still needs runtime verification and UE audio routing confirmation.
- Next recommended step: Create a minimal actor/Blueprint flow to set LUSID scene, ADM path, TransLab layout, enable `bUseSpatialRootHostBus`, and confirm `renderHostBlock()` drives `USpatialRootRenderBusComponent`.

## Next Recommended Tasks

1. Create a minimal level/actor or Blueprint binding for `USpatialRootSubsystem`, `USpatialRootRenderBusComponent`, and `USpatialRootTestToneComponent`.
2. Locate a known-good ADM/BW64 plus LUSID scene pair for the TransLab layout.
3. Start `EngineSession` in editor and document whether AlloLib device startup succeeds or fails under Unreal.
4. Query Unreal sample rate and output channel count for diagnostics.
5. Keep the Spatial Root submodule pinned intentionally when updating the dependency.

## Do Not Repeat

- Failed attempt: Looking for skill docs under `docs/` before they exist there.
- Why it failed: The initial repository has `UE_ROOT_AGENT_SKILL.md` and `SPATIAL_ROOT_AGENT_SKILL.md` at the repo root.
- What to do instead: Read the root-level files until the repository structure is normalized.
