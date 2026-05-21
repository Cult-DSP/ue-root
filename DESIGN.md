# ue-root Design Reference

`ue-root` is an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

The first goal is not visualization. The first goal is to prove that Unreal can host Spatial Root as an embedded spatial audio runtime, load ADM/BW64 input, load a Spatial Root loudspeaker layout, start playback, expose runtime controls, and route Spatial Root-rendered audio into Unreal's audio output path.

This repository is intended to live under the Cult DSP GitHub organization as a reference example, not as a finished production plugin.

## Project Status

Experimental reference integration / template.

Initial target:

- macOS only
- Lucian's development machine only
- ADM/BW64 input only
- Spatial Root API is treated as locked
- Unreal owns final audio output
- No visualization in the first milestone
- Hybrid C++ plus Blueprint/UMG architecture

## Core Claim

`ue-root` demonstrates that Spatial Root can operate as an embedded spatial audio runtime inside a game-engine host.

Spatial Root remains responsible for:

- ADM/BW64 ingest, if supported by the existing engine/session path
- LUSID or internal scene representation
- speaker layout loading
- spatial rendering
- DBAP/runtime parameter behavior
- playback state

Unreal remains responsible for:

- application shell
- file selection
- UI controls
- diagnostics
- final audio output path
- future visualization or game-engine interaction

## First Milestone

The first milestone should allow the user to:

1. Open the Unreal app.
2. Choose an ADM/BW64 input file.
3. Choose a Spatial Root layout JSON file.
4. Click Start.
5. Hear audio from the Unreal app.
6. Move runtime controls.
7. Toggle pause/resume.
8. Stop playback.
9. Inspect diagnostics.

Required controls:

- ADM/BW64 input path
- layout JSON path
- Start
- Pause
- Resume
- Stop
- master gain dB
- DBAP focus
- speaker mix
- sub mix
- live update toggle
- Apply params button

Required diagnostics:

- engine initialized
- ADM path
- layout path
- ADM loaded status
- layout loaded status
- running / paused / stopped
- Unreal sample rate, if accessible
- Unreal output channel count, if accessible
- Spatial Root required output channel count, if accessible
- rendered channel count, if accessible
- xrun / underrun count, if available
- last error string
- last operation
- whether audio is routed through Unreal's audio path or falling back to another path

## Preferred Architecture

```text
UERoot Unreal App
  └── Plugins/
      └── SpatialRootHost/
          ├── SpatialRootHost.uplugin
          ├── Source/
          │   ├── SpatialRootHost/
          │   │   ├── SpatialRootSubsystem
          │   │   ├── SpatialRootComponent
          │   │   ├── SpatialRootBridge
          │   │   ├── SpatialRootAudioSource / Generator
          │   │   └── SpatialRootDiagnostics
          │   └── ThirdParty/
          │       └── SpatialRoot/
          └── Content/
              └── UI/
```

Conceptual flow:

```text
Unreal UI
    ↓
SpatialRootSubsystem / Component
    ↓
SpatialRootBridge
    ↓
Spatial Root EngineSession API
    ↓
Spatial Root rendered PCM, if available
    ↓
Unreal procedural source / generator
    ↓
Unreal submix / master output
    ↓
selected Unreal audio device
```

## Audio Integration Target

The preferred proof of concept is:

```text
Spatial Root embedded engine
    exposes rendered PCM somehow
        ↓
Unreal audio generator / source
        ↓
Audio Bus or Submix
        ↓
Unreal output device
```

This should not be confused with a fake "Unreal virtual device." Unreal has audio buses, source buses, submixes, and procedural audio sources, but Spatial Root samples must first enter Unreal's audio graph.

Therefore, the key technical question is:

```text
Can the current locked Spatial Root API provide rendered PCM blocks without opening its own hardware audio device?
```

If yes, `ue-root` should route those buffers into Unreal's audio path.

If no, the first pass should:

- build the plugin skeleton,
- build a test procedural audio source,
- document the missing Spatial Root render-buffer access,
- avoid pretending the integration is complete.

## Spatial Root API Policy

The Spatial Root API is locked for this task.

Agents should not refactor Spatial Root to make this work.

Agents may:

- inspect EngineSession,
- use existing public or internal APIs,
- create a small Unreal-facing wrapper,
- document missing APIs,
- propose future API needs.

Agents should not:

- redesign Spatial Root,
- add a new renderer backend unless explicitly asked,
- change existing engine behavior,
- break current standalone Spatial Root builds.

## EngineSession vs C ABI

The first `ue-root` version should highlight the existing `EngineSession` API.

Preferred first pass:

```text
Unreal C++ bridge calls EngineSession directly.
```

Future cross-host work may introduce a C ABI wrapper:

```text
SpatialRootHost C ABI
    ↓
Unity
TouchDesigner
Max/MSP
Python
other non-C++ hosts
```

Do not implement the C ABI in the first pass unless direct C++ inclusion becomes impossible.

## Dependency Strategy

The first pass should use Spatial Root from source as a submodule or expected local checkout.

Preferred development posture:

- macOS-only
- local development machine only
- source checkout/submodule preferred over prebuilt binary packaging
- no cross-platform package guarantees yet

The agent should document whether Spatial Root is:

- present as a submodule,
- expected at a local path,
- copied into `ThirdParty/SpatialRoot`,
- or temporarily unresolved.

## ADM/BW64 Scope

The first milestone is ADM/BW64 input only.

The UI should not require the user to select a LUSID scene for v0.1 unless Spatial Root's current EngineSession path requires ADM to be transcoded to LUSID first.

If ADM/BW64 cannot be loaded directly through the current locked Spatial Root API, the agent should document the required ingest path rather than inventing a new one.

## Runtime Parameter Policy

The UI should support both live and explicit parameter updates:

- Sliders may update live when "Live Update" is enabled.
- The "Apply Params" button should send all current runtime values explicitly.
- Scene/layout loading should use explicit buttons, not automatic live updates.

Initial runtime parameters:

- master gain dB
- DBAP focus
- speaker mix
- sub mix
- paused/running state

## Diagnostics Policy

Diagnostics are part of the core first milestone, not polish.

The app should expose the most important state even if the audio path is not fully solved.

Required diagnostics include:

- engine initialized
- ADM path
- layout path
- ADM loaded status
- layout loaded status
- running / paused / stopped
- Unreal sample rate, if accessible
- Unreal output channel count, if accessible
- Spatial Root required output channel count, if accessible
- rendered channel count, if accessible
- xrun / underrun count, if available
- last error
- last operation
- current audio path mode

Audio path mode should be one of:

- UE procedural source
- Spatial Root owns device
- stub/test generator only
- failed/not connected
- unknown

## Deferred Work

Do not implement these in the first milestone:

- LUSID object visualization
- speaker layout visualization
- OSC controller mode
- external process launching
- Windows support
- cross-platform packaging
- TouchDesigner `td-root`
- Unity integration
- production Unreal plugin hardening
- public binary release
- C ABI wrapper
- deep rewrite of Spatial Root for Unreal Audio Mixer
- marketplace dependencies

## Example Content

Example ADM/BW64 and Atmos-related material can be linked from:

https://huggingface.co/datasets/lucianparisi/atmos-data/tree/main

The repo should not assume that large audio assets are committed directly unless explicitly approved.

## Documentation Requirements

The repository should include or grow toward:

- `README.md`
- `DESIGN.md`
- `AGENTS.md`
- `ROADMAP.md`
- `docs/TASK_LOG.md`
- `docs/ENGINE_SESSION_API.md`
- `docs/UNREAL_AUDIO_PATH.md`
- `docs/DIAGNOSTICS.md`
- `docs/FUTURE_C_ABI.md`
- `docs/EXAMPLE_CONTENT.md`

All repository documents must be valid standalone Markdown files. Do not include ChatGPT artifact labels, `text id=...` wrappers, writing-block syntax, or fragmented response formatting. Use normal Markdown headings, paragraphs, lists, and fenced code blocks only where useful.

## Success Criteria

Minimum success:

- Unreal project opens.
- `SpatialRootHost` plugin exists.
- `SpatialRootBridge` skeleton exists.
- UI skeleton exists.
- diagnostics panel exists.
- README explains setup and limitations.
- documentation clearly states whether Spatial Root can currently feed Unreal's audio path.

Useful success:

- ADM/BW64 path and layout path can be selected.
- EngineSession can be initialized from Unreal.
- Start/pause/resume/stop are wired.
- runtime params are wired.
- diagnostics update.

Strong success:

- Spatial Root-rendered audio enters Unreal's audio path.
- UE device selection controls final output.
- runtime params audibly affect rendering.
- channel count and routing behavior are documented.

## Paper Framing

`ue-root` should support this paper-level claim:

Spatial Root can be embedded as a spatial audio runtime inside a game-engine host. Unreal provides application control, UI, diagnostics, and the eventual output path, while Spatial Root provides ADM/BW64 scene handling, layout-aware rendering, and runtime spatial audio behavior.

If the first milestone only reaches the skeleton or test-generator phase, the documentation should clearly state the remaining technical requirement: a Spatial Root render-buffer path suitable for Unreal's procedural audio source/generator model.
