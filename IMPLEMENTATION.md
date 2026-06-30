# Implementation Notes

`ue-root` currently uses a source-controlled runtime path instead of binary Unreal assets for the first validation pass.

## Runtime Flow

1. `DefaultEngine.ini` sets `AUERootGameMode` as the default game mode.
2. `AUERootGameMode` spawns `ASpatialRootHostTestActor` into the default OpenWorld template map.
3. `AUERootGameMode` creates `UUERootControlPanel` and adds it to the viewport.
4. `UUERootControlPanel` lets the user edit ADM/BW64, LUSID scene, and layout paths, then start the test tone or Spatial Root host-bus path.
5. `ASpatialRootHostTestActor` owns `USpatialRootTestToneComponent` and `USpatialRootRenderBusComponent`.
6. `USpatialRootRenderBusComponent` pulls interleaved PCM from `USpatialRootBridge::RenderHostAudio()` when host-bus mode is enabled.

## Default Candidate Inputs

- ADM/BW64 candidate: `/Users/lucian/projects/spatialroot/sourceData/CANYON-ATMOS-LFE.wav`
- LUSID scene candidate: `/Users/lucian/projects/spatialroot/sourceData/lusid_package/scene.lusid.json`
- Layout: TransLab, resolved from the in-repo Spatial Root checkout

These are not confirmed-good until an interactive editor run verifies loading and audible output.

## Validation Status

`UERootEditor` builds successfully with the native game mode and control panel.

An unattended `UnrealEditor-Cmd -game -NullRHI -Unattended -ExecCmds=quit` launch did not exit cleanly and cannot validate audio output. The next useful validation is an interactive editor/PIE run using the native control panel:

1. Press `Start Tone`.
2. Confirm audible Unreal synth output.
3. Press `Start`.
4. Confirm `renderHostBlock()` feeds Unreal audio.
5. Record sample-rate and output-channel diagnostics.
