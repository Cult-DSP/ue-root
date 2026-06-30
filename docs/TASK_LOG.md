# Task Log

## Current State

The repository has a minimal Unreal project shell at `Unreal/UERoot.uproject` and a `SpatialRootHost` runtime plugin. Spatial Root is tracked as a submodule under `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, pointed at the canonical Cult-DSP `host-render` branch.

`SpatialRootHost` links `EngineSessionCore` from the Spatial Root submodule build artifact and compiles with real `EngineSession` lifecycle calls. The canonical Spatial Root `host-render` branch includes the Internal Host Bus API that renders PCM into a host-owned interleaved buffer without opening a device. The TransLab layout is the benchmark target, parsed as 18 output channels. `USpatialRootRenderBusComponent` can pull from the host bus via `USpatialRootBridge`, but this flow still needs interactive editor/audio verification after the dependency repoint.

`ASpatialRootHostTestActor` provides a source-controlled runtime harness for the test tone and host-bus path. `AUERootGameMode` is now the default game mode and spawns the harness plus a native C++ `UUERootControlPanel` into the default OpenWorld template map. Initial `Unreal/Content/UI` and `Unreal/Content/Maps` placeholders exist, but no binary `.uasset` map or widget exists yet.

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

- Fact: `UERootEditor` still builds after onboarding verification.
- Evidence: UnrealBuildTool completed two actions and linked `UnrealEditor-SpatialRootHost.dylib`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: During onboarding, the required Unreal content/UI layer was not present yet.
- Evidence: `Unreal/Content` did not exist before the placeholder folders were added.
- File or command: `find Unreal/Content -maxdepth 5 -type f`

- Fact: A minimal runtime actor harness now exists for editor validation.
- Evidence: `ASpatialRootHostTestActor` owns `USpatialRootTestToneComponent` and `USpatialRootRenderBusComponent`, obtains `USpatialRootBridge` from `USpatialRootSubsystem`, and exposes Blueprint-callable start/stop methods.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/SpatialRootHost/Public/SpatialRootHostTestActor.h`; `Unreal/Plugins/SpatialRootHost/Source/SpatialRootHost/Private/SpatialRootHostTestActor.cpp`

- Fact: A source-controlled native control panel now exists.
- Evidence: `UUERootControlPanel` builds a UMG widget tree at runtime with path fields, Start/Pause/Resume/Stop controls, runtime sliders, live update, Apply Params, and diagnostics text.
- File or command: `Unreal/Source/UERoot/UERootControlPanel.h`; `Unreal/Source/UERoot/UERootControlPanel.cpp`

- Fact: The project now auto-spawns the harness and native panel in the default map.
- Evidence: `AUERootGameMode` spawns `ASpatialRootHostTestActor` and `UUERootControlPanel`; `DefaultEngine.ini` sets `GlobalDefaultGameMode=/Script/UERoot.UERootGameMode`.
- File or command: `Unreal/Source/UERoot/UERootGameMode.*`; `Unreal/Config/DefaultEngine.ini`

- Fact: Initial Unreal content folders now exist as source-controlled placeholders.
- Evidence: `Unreal/Content/UI/.gitkeep` and `Unreal/Content/Maps/.gitkeep` were added.
- File or command: `rg --files --hidden Unreal/Content`

- Fact: `UERootEditor` builds successfully with the native game mode and control panel.
- Evidence: UnrealBuildTool compiled `UERootControlPanel.cpp`, `UERootGameMode.cpp`, relinked `UnrealEditor-SpatialRootHost.dylib`, and linked `UnrealEditor-UERoot.dylib`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: An unattended command-line runtime launch was attempted but did not complete.
- Evidence: `UnrealEditor-Cmd -game -NullRHI -Unattended -ExecCmds=quit` did not exit cleanly and emitted macOS service warnings before being terminated.
- File or command: `/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor-Cmd /Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -game -NullRHI -Unattended -ExecCmds=quit`

- Fact: Spatial Root realtime engine builds successfully from the in-repo clone.
- Evidence: Build completed and produced `libEngineSessionCore.a` plus `spatialroot_realtime`.
- File or command: `./build.sh --engine-only`

- Fact: `SpatialRootHost` links against `EngineSessionCore` without modifying the Spatial Root submodule.
- Evidence: `UERootEditor` build succeeded after adding Spatial Root static libraries and frameworks to `SpatialRootHost.Build.cs`.
- File or command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`

- Fact: The benchmark layout is TransLab, requiring 18 output channels.
- Evidence: `translab-sono-layout.json` contains 16 speakers and 2 subwoofers with channels `0` through `17`.
- File or command: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/speaker_layouts/translab-sono-layout.json`

- Fact: A UE-side render-bus component exists and can request Spatial Root PCM through the bridge.
- Evidence: `USpatialRootRenderBusComponent` outputs through `USynthComponent` and calls `USpatialRootBridge::RenderHostAudio()` when `bUseSpatialRootHostBus` is enabled.
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

- Blocker: Audio output has not been auditioned in an interactive Unreal editor/session.
- Evidence: The project builds and has a native runtime panel, but the unattended `-NullRHI` launch did not complete and cannot verify audible output.
- Possible workaround: Launch the editor interactively, press `Start Tone`, then press `Start` for the host-bus path and inspect diagnostics.

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

- Task: Onboarded against `AGENTS.md` and refreshed remaining-task state.
- Result: Verified the repo shell, plugin, Spatial Root submodule pin, preferred Unreal path, preferred source-data path, missing `Unreal/Content`, and a successful `UERootEditor` build.
- Files changed: `docs/TASK_LOG.md`

- Task: Added a minimal source-controlled runtime test harness.
- Result: `ASpatialRootHostTestActor` compiles and exposes test-tone and Spatial Root host-bus start/stop methods, plus editable ADM/BW64, LUSID scene, layout, channel count, block size, and sample-rate fields.
- Files changed: `SpatialRootHostTestActor.h`, `SpatialRootHostTestActor.cpp`, `README.md`, `docs/UNREAL_AUDIO_PATH.md`, `docs/TASK_LOG.md`, `Unreal/Content/UI/.gitkeep`, `Unreal/Content/Maps/.gitkeep`

- Task: Added source-controlled runtime UI and auto-spawn flow.
- Result: `AUERootGameMode` now spawns `ASpatialRootHostTestActor` and `UUERootControlPanel`; the panel exposes required paths, transport controls, runtime params, live update, Apply Params, and diagnostics without binary UI assets.
- Files changed: `Unreal/Source/UERoot/UERoot.Build.cs`, `Unreal/Source/UERoot/UERootGameMode.*`, `Unreal/Source/UERoot/UERootControlPanel.*`, `Unreal/Config/DefaultEngine.ini`, docs updates

- Task: Attempted unattended runtime validation.
- Result: Build succeeded, but `UnrealEditor-Cmd -game -NullRHI -Unattended -ExecCmds=quit` did not exit cleanly and was terminated; audible validation remains interactive.
- Files changed: `docs/TASK_LOG.md`, `docs/UNREAL_AUDIO_PATH.md`

- Task: Refreshed docs for native harness/control-panel state.
- Result: `ROADMAP.md`, `DESIGN.md`, and `docs/ENGINE_SESSION_API.md` now reflect that host-bus render buffers, the native runtime panel, and default game-mode spawning exist; docs still correctly mark audible editor validation as pending.
- Files changed: `ROADMAP.md`, `DESIGN.md`, `docs/ENGINE_SESSION_API.md`, `docs/UNREAL_AUDIO_PATH.md`, `docs/TASK_LOG.md`

## Local Unreal Setup

- Unreal path checked: `/Users/lucian/UE_5.7`
- Exists: Yes
- Editor launch path: `/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app`
- Project path: `/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject`
- Build command: `/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh UERootEditor Mac Development -project=/Users/lucian/projects/cultProjects/ue-root/Unreal/UERoot.uproject -WaitMutex`
- Build result: Succeeded after adding `AUERootGameMode` and `UUERootControlPanel`.
- Plugin status: Compiles with `EngineSessionCore` linked.
- Spatial Root dependency status: In-repo clone found and built; linked into the Unreal plugin.
- In-repo Spatial Root checkout: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot`, canonical `host-render`, commit `4e04d37`, recursive submodules initialized.
- Audio test status: Test-tone synth and render-bus synth components compile; native control panel compiles; not yet auditioned in an interactive editor session.
- Next step: Launch the editor interactively, use the native control panel to validate test-tone output, then validate host-bus output.

## Local Test Data

- sourceData path: `/Users/lucian/projects/spatialroot/sourceData`
- Exists: Yes
- ADM/BW64 candidates: `CANYON-ATMOS-LFE.wav`, `ASCENT-ATMOS-LFE.wav`, `OFFERING-ATMOS-LFE.wav`, `SWALE-ATMOS-LFE.wav`, `EDEN-ATMOS-MIX-LFE.wav`, `sphere-ascent-atmos.wav`, `360RA_test.wav`
- Layout JSON candidates: TransLab benchmark selected at `spatial_engine/speaker_layouts/translab-sono-layout.json`; other candidates include `stereo.json`, `example_layouts/5_1.json`, `example_layouts/quad_4.json`, and `example_layouts/ring8_top4.json`.
- LUSID candidates: `sourceData/lusid_package/scene.lusid.json`, `sourceData/qgoo_LUSID/scene.lusid.json`
- Selected first ADM/BW64 test file: Candidate default is `/Users/lucian/projects/spatialroot/sourceData/CANYON-ATMOS-LFE.wav`; not confirmed-good yet.
- Selected first layout file: `spatial_engine/speaker_layouts/translab-sono-layout.json`
- Open questions: Need interactive confirmation that `CANYON-ATMOS-LFE.wav` plus `sourceData/lusid_package/scene.lusid.json` plus TransLab is a valid runtime test pair.

## UE Audio Investigation

- Audio entry point tested: Not auditioned yet; `USynthComponent` test tone and render bus compile and are reachable through the native panel.
- Test tone/silence status: Test tone and 18-channel render-bus source code compiled successfully; interactive audio test pending.
- Submix routing status: Not attempted.
- UE sample rate: Unknown.
- UE output channel count: Unknown.
- Spatial Root render-buffer path: Internal Host Bus API now available.
- Spatial Root engine-only build: Succeeded from the canonical `host-render` submodule after clearing the old CMake cache from the previous submodule path.
- Blocker: Unreal runtime validation still pending (test tone and host bus path not auditioned in editor yet).
- Next step: Use the auto-spawned native panel to verify test-tone output, then host-bus `renderHostBlock()` output in editor.

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
- Next recommended step: Launch the project interactively, use the default native panel candidate paths, then confirm `renderHostBlock()` drives `USpatialRootRenderBusComponent`.

## Next Recommended Tasks

1. Launch the project interactively in Unreal Editor or PIE; the default game mode should spawn `ASpatialRootHostTestActor` and `UUERootControlPanel`.
2. Press `Start Tone` and verify audible Unreal synth output.
3. Confirm or adjust the default ADM/BW64, LUSID scene, and TransLab layout paths in the panel.
4. Press `Start` and verify whether `renderHostBlock()` produces audible output through Unreal's synth/procedural path.
5. Query or confirm Unreal sample rate and actual output channel count, then document any mismatch between requested 18-channel output and the selected device/mixer output.
6. If desired after validation, author binary `.uasset` map/widget assets under `Unreal/Content/Maps` and `Unreal/Content/UI`.
7. Keep the Spatial Root submodule pinned intentionally when updating the dependency.

## Do Not Repeat

- Failed attempt: Looking for skill docs under `docs/` before they exist there.
- Why it failed: The initial repository has `UE_ROOT_AGENT_SKILL.md` and `SPATIAL_ROOT_AGENT_SKILL.md` at the repo root.
- What to do instead: Read the root-level files until the repository structure is normalized.

- Failed attempt: Using unattended `UnrealEditor-Cmd -game -NullRHI -Unattended -ExecCmds=quit` as runtime validation.
- Why it failed: The process did not exit cleanly and emitted macOS service warnings; `NullRHI` also cannot prove audible audio output.
- What to do instead: Use an interactive editor/PIE run for audio validation.
