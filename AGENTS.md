# AGENTS.md

This file defines working instructions for autonomous agents contributing to `ue-root`.

`ue-root` is an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

The first milestone is intentionally narrow: macOS-only, local-machine-only, ADM/BW64 input only, no visualization, and no production packaging.

Before local Unreal implementation work, read `docs/UE_ROOT_AGENT_SKILL.md`. Use `/Users/lucian/UE_5.7` as the preferred Unreal Engine path and `/Users/lucian/projects/spatialroot/sourceData` as the preferred local test-data path. Document any deviation in `docs/TASK_LOG.md`.

Before Spatial Root integration work, read `docs/SPATIAL_ROOT_AGENT_SKILL.md`. Treat the Spatial Root source as the source of truth, use `/Users/lucian/projects/spatialroot` as the preferred local repo path, and document all EngineSession, ADM/BW64, layout, runtime, and render-buffer findings in `docs/ENGINE_SESSION_API.md` and `docs/TASK_LOG.md`.

## Project Intent

The goal is to prove that Unreal can host Spatial Root as an embedded spatial audio runtime.

The target user flow is:

1. Open the Unreal app.
2. Choose an ADM/BW64 input file.
3. Choose a Spatial Root layout JSON file.
4. Click Start.
5. Hear audio from the Unreal app.
6. Adjust runtime controls.
7. Pause/resume/stop.
8. Inspect diagnostics.

Do not turn this into a visualization project yet.

## Hard Constraints

- Spatial Root API is locked.
- Do not refactor Spatial Root.
- Do not implement visualization.
- Do not build an OSC controller.
- Do not launch a separate Spatial Root process.
- Do not target Windows in the first pass.
- Do not add marketplace dependencies.
- Do not overbuild the UI.
- Do not claim mixer-native integration works unless it actually works.
- Do not hide failed audio-routing attempts. Document them.
- Do not break current standalone Spatial Root builds.

## Preferred Architecture

Use a hybrid Unreal architecture:

- C++ owns the Spatial Root bridge and audio integration.
- Blueprint/UMG owns the control UI.
- The plugin should be reusable later in other Unreal projects.

Recommended structure:

```text
ue-root/
  README.md
  ROADMAP.md
  DESIGN.md
  AGENTS.md
  docs/
  Unreal/
    UERoot.uproject
    Config/
    Content/
      UI/
      Maps/
    Plugins/
      SpatialRootHost/
        SpatialRootHost.uplugin
        Source/
          SpatialRootHost/
            SpatialRootHost.Build.cs
            Public/
            Private/
          ThirdParty/
            SpatialRoot/
              SpatialRoot.Build.cs
              spatialroot/
```

## First Onboarding Tasks

Every new agent should start by doing these tasks in order:

1. Read `DESIGN.md`.
2. Read this `AGENTS.md`.
3. Inspect the current repository tree.
4. Check whether an Unreal project already exists.
5. Check whether `Plugins/SpatialRootHost` already exists.
6. Check whether Spatial Root is present as a submodule, local checkout, or documented external path.
7. Search for existing EngineSession usage in Spatial Root.
8. Search for any existing render-to-buffer, offline render, processBlock, renderBlock, callback, internal render bus, or host-pull audio path.
9. Search for current Spatial Root ADM/BW64 load path and layout load path.
10. Write findings into the project memory section before implementing.

## Required Investigation Before Audio Work

Before attempting Unreal audio routing, determine:

- Can EngineSession be constructed from an external C++ host?
- Can EngineSession load ADM/BW64 directly, or does it require a LUSID conversion path first?
- Can EngineSession load a layout JSON path directly?
- Can runtime params be set directly?
- Can playback be started, paused, resumed, and stopped cleanly?
- Can Spatial Root produce rendered PCM blocks without opening a hardware audio device?
- If yes, where is that API?
- If no, what minimal future API would be needed?
- Can Unreal procedural audio source/generator output a test tone?
- Can the test source be routed to a submix/master output?
- Can Unreal report sample rate and output channel count?

Document answers before claiming success.

## Audio Path Decision Rule

Preferred path:

```text
Spatial Root rendered PCM
    ↓
Unreal procedural audio source / generator
    ↓
Unreal submix or audio bus
    ↓
Unreal output device
```

If Spatial Root has no render-buffer path:

1. Do not fake the integration.
2. Build the Unreal side with a test tone/stub generator.
3. Document the exact missing Spatial Root API.
4. Keep the EngineSession wrapper and UI skeleton useful.
5. Optionally document whether Spatial Root can run inside the Unreal process while owning its own device, but do not treat that as the preferred final design.

## Minimal SpatialRootBridge Surface

Keep the bridge small.

Suggested methods:

```cpp
bool Initialize();
bool LoadAdmBwf(const FString& AdmPath);
bool LoadLayout(const FString& LayoutPath);
bool Start();
void Pause();
void Resume();
void Stop();
void Shutdown();

void SetMasterGainDb(float Db);
void SetDbapFocus(float Focus);
void SetSpeakerMix(float Mix);
void SetSubMix(float Mix);

FString GetLastError() const;
FSpatialRootDiagnostics GetDiagnostics() const;
```

Do not expose large parts of Spatial Root internals to Unreal.

## Required UI

Create a simple control panel with:

- ADM/BW64 file path
- layout JSON file path
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
- diagnostics window

The UI can be basic. It should be clear and reliable, not polished.

## Required Diagnostics

Expose as much as possible:

- engine initialized
- ADM path
- layout path
- ADM loaded status
- layout loaded status
- running / paused / stopped
- Unreal sample rate, if available
- Unreal output channel count, if available
- Spatial Root required output channel count, if available
- rendered channel count, if available
- xrun / underrun count, if available
- last error
- last operation
- current audio path mode:
  - UE procedural source
  - Spatial Root owns device
  - stub/test generator only
  - failed/not connected

## Recursive Task and Memory Updating

Agents must maintain a lightweight project memory.

Use this format in `docs/TASK_LOG.md`. If the file does not exist, create it.

```markdown
# Task Log

## Current State

Short summary of what currently works.

## Confirmed Facts

- Fact:
- Evidence:
- File or command:

## Open Questions

- Question:
- Why it matters:
- Suggested next step:

## Blockers

- Blocker:
- Evidence:
- Possible workaround:

## Last Completed Tasks

- Task:
- Result:
- Files changed:

## Next Recommended Tasks

1.
2.
3.

## Do Not Repeat

- Failed attempt:
- Why it failed:
- What to do instead:
```

Update `docs/TASK_LOG.md` at the end of every meaningful work session.

If a task fails, record the failure. Do not delete the history just because a later attempt succeeds.

## Documentation Update Rules

When making changes, update docs in the same commit or work session.

Update:

- `README.md` when setup or user-facing behavior changes.
- `DESIGN.md` when architecture changes.
- `docs/TASK_LOG.md` after each work session.
- `docs/UNREAL_AUDIO_PATH.md` when audio routing changes.
- `docs/ENGINE_SESSION_API.md` when EngineSession usage is discovered or changed.
- `docs/DIAGNOSTICS.md` when diagnostic fields are added or removed.

## README Requirements

The README should clearly state:

- `ue-root` is experimental.
- v0.1 is macOS-only and local-machine-only.
- The project embeds Spatial Root in Unreal.
- The first goal is ADM/BW64 + layout loading and audio playback.
- Visualization is deferred.
- Cross-platform packaging is deferred.
- Example content is available at:
  https://huggingface.co/datasets/lucianparisi/atmos-data/tree/main

## Coding Style

- Prefer small C++ wrapper classes.
- Keep Unreal-facing code readable.
- Avoid clever abstractions.
- Comment the bridge between Unreal and Spatial Root carefully.
- Keep Blueprint exposure minimal and intentional.
- Do not bury important behavior in UMG graphs without documentation.
- Prefer explicit status/error reporting over silent failure.
- Avoid broad refactors.

## Stop Conditions

Stop and document findings if:

- Spatial Root cannot be linked into Unreal.
- EngineSession cannot be constructed from Unreal.
- ADM/BW64 loading is not available through the current API.
- no render-buffer path exists for Unreal audio output.
- Unreal audio generator cannot be made to produce even a test tone.
- Unreal cannot report or use the needed output channel configuration.

A stopped task is still useful if the blocker is documented clearly.

## First Agent Mission

The first agent should produce:

1. Repo skeleton.
2. Unreal project shell.
3. `SpatialRootHost` plugin skeleton.
4. `SpatialRootBridge` skeleton.
5. UMG/control UI skeleton or documented placeholder.
6. Diagnostics model skeleton.
7. Audio path investigation notes.
8. `docs/TASK_LOG.md`.
9. README setup draft.
10. Clear statement of whether Spatial Root can currently feed Unreal's audio path.

Minimum acceptable result:

- The repo opens.
- The plugin skeleton exists.
- The docs clearly explain the next technical step.

Ideal result:

- The Unreal app starts an embedded Spatial Root session.
- Spatial Root-rendered audio reaches Unreal's output path.
- Runtime controls work.
- Diagnostics make routing and failure modes visible.

## Markdown File Rule

All repository documents must be valid standalone Markdown files.

Do not include:

- ChatGPT artifact labels
- `text id=...` wrappers
- writing-block syntax
- fragmented response formatting
- accidental nested/broken code fences

Use normal Markdown headings, paragraphs, lists, and fenced code blocks only where useful.

If a Markdown file needs to show a fenced code block inside another fenced block, use four backticks for the outer block or rewrite the example as an indented block.
