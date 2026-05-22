# `ue-root` Agent Skill: Local Unreal, Test Data, and UE Audio

This skill document is for autonomous agents working on `ue-root`, an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

This document combines three required skill areas:

1. Lucian's local Unreal Engine setup.
2. Lucian's local Spatial Root test-data directory.
3. Unreal audio integration concepts needed for `ue-root`.

Use this document together with:

- `DESIGN.md`
- `AGENTS.md`
- `docs/UNREAL_AGENT_SKILLS.md`

If another setup document conflicts with this file, prefer this file for the first macOS-local `ue-root` implementation pass.

## Project Scope

The first `ue-root` milestone is:

- macOS only
- local development machine only
- Unreal installed at `/Users/lucian/UE_5.7`
- Spatial Root test data available at `/Users/lucian/projects/spatialroot/sourceData`
- ADM/BW64 input only for the first pass
- layout JSON input required
- no visualization
- no OSC controller mode
- no external Spatial Root process
- no Windows or Linux support
- no production packaging
- no Spatial Root API refactor

The desired first user flow is:

1. Open the Unreal app.
2. Choose an ADM/BW64 input file.
3. Choose a Spatial Root layout JSON file.
4. Click Start.
5. Hear audio from the Unreal app.
6. Adjust runtime controls.
7. Pause/resume/stop.
8. Inspect diagnostics.

## Local Unreal Engine Path

Lucian's preferred local Unreal Engine path is:

```text
/Users/lucian/UE_5.7
```

Agents must check this path before guessing any other Unreal installation.

### Required Unreal Path Checks

Run checks equivalent to:

```bash
ls "/Users/lucian/UE_5.7"
ls "/Users/lucian/UE_5.7/Engine"
ls "/Users/lucian/UE_5.7/Engine/Binaries/Mac"
ls "/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac"
```

Look for:

```text
Engine/Binaries/Mac/UnrealEditor.app
Engine/Binaries/Mac/UnrealEditor
Engine/Build/BatchFiles/Mac/Build.sh
Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh
```

If any expected path is missing, document the actual contents in `docs/TASK_LOG.md` and stop before inventing another path.

## Launching Unreal

Try the app bundle first:

```bash
open "/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app"
```

If command-line launch is needed, try:

```bash
"/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
```

or, if present:

```bash
"/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor"
```

## Opening the `ue-root` Project

Expected project path:

```text
ue-root/Unreal/UERoot.uproject
```

Use:

```bash
open -a "/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app" "/path/to/ue-root/Unreal/UERoot.uproject"
```

or:

```bash
"/Users/lucian/UE_5.7/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor" "/path/to/ue-root/Unreal/UERoot.uproject"
```

Replace `/path/to/ue-root` with the actual repository path.

## Generating Project Files

If project files are needed, try:

```bash
"/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" -project="/path/to/ue-root/Unreal/UERoot.uproject" -game
```

If this script does not exist or fails, record the error in `docs/TASK_LOG.md`.

## Building the Project

If command-line build is needed, try:

```bash
"/Users/lucian/UE_5.7/Engine/Build/BatchFiles/Mac/Build.sh" UERootEditor Mac Development -project="/path/to/ue-root/Unreal/UERoot.uproject" -WaitMutex
```

If the project name changes, update `UERootEditor` accordingly.

If the target does not exist yet, create the minimal Unreal project/module first or document that the project shell is incomplete.

## Local Spatial Root Test Data

Lucian's local Spatial Root test-data path is:

```text
/Users/lucian/projects/spatialroot/sourceData
```

Agents should use this path for first-pass local testing.

Do not copy large test files into `ue-root` unless explicitly approved.

Do not assume a specific file name. Inspect the directory.

### Required Test-Data Checks

Run checks equivalent to:

```bash
ls "/Users/lucian/projects/spatialroot/sourceData"
find "/Users/lucian/projects/spatialroot/sourceData" -maxdepth 4 -type f
```

Then narrow to likely audio, ADM/BW64, LUSID, and layout files:

```bash
find "/Users/lucian/projects/spatialroot/sourceData" -maxdepth 6 -type f \( \
  -iname "*.wav" -o \
  -iname "*.bw64" -o \
  -iname "*.bwf" -o \
  -iname "*.json" -o \
  -iname "*.lusid.json" \
\)
```

If this command is too broad, use smaller `find` commands and document the discovered files.

### File Classification

When inspecting `sourceData`, classify files into:

- ADM/BW64 candidate audio files
- ordinary WAV files
- layout JSON files
- LUSID scene/package files
- metadata/report files
- unknown files

Do not treat every `.wav` file as ADM/BW64. If possible, identify whether an audio file contains ADM metadata using existing Spatial Root tools, existing CULT tools, or documented file names.

### Expected First Test Pair

The first `ue-root` run needs:

```text
ADM/BW64 input file
Spatial Root layout JSON file
```

If a valid pair cannot be identified automatically, document candidate files and stop before guessing.

### Task Log Entry

At the end of test-data inspection, update `docs/TASK_LOG.md` with:

```markdown
## Local Test Data

- sourceData path:
- Exists:
- ADM/BW64 candidates:
- Layout JSON candidates:
- LUSID candidates:
- Selected first ADM/BW64 test file:
- Selected first layout file:
- Open questions:
```

## Spatial Root Dependency Location

The agent should not assume Spatial Root is vendored into `ue-root`.

Check for one of:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/spatialroot-embedding
ThirdParty/SpatialRoot
spatialroot
../spatialroot
../SpatialRoot
/Users/lucian/projects/spatialroot
```

The most likely local Spatial Root project path is:

```text
/Users/lucian/projects/spatialroot
```

If found, document:

- branch name
- latest commit hash
- whether it builds independently
- whether `EngineSession` headers are visible
- whether sourceData exists

Useful commands:

```bash
cd "/Users/lucian/projects/spatialroot"
git status --short
git branch --show-current
git rev-parse --short HEAD
```

Do not modify Spatial Root unless explicitly instructed. The Spatial Root API is locked for this task.

## Required Spatial Root API Investigation

Before wiring Unreal audio, inspect Spatial Root for:

- `EngineSession`
- ADM/BW64 load path
- layout load path
- runtime parameter setters
- start/pause/resume/stop behavior
- shutdown order
- render-buffer or host-pull audio path

Search terms:

```text
EngineSession
LoadScene
loadScene
LoadLayout
loadLayout
ADM
BW64
BWF
RuntimeParams
setPaused
pause
resume
start
shutdown
renderBlock
processBlock
offline
callback
AudioCallback
buffer
interleaved
outputBuffer
render bus
internal bus
device buffer
```

The agent must determine whether Spatial Root can provide rendered PCM blocks without opening its own audio device.

## UE Audio Mental Model

This distinction is mandatory:

```text
Audio source/generator = how samples enter Unreal.
Submix / Audio Bus = how Unreal routes or processes samples after they are already inside Unreal.
```

An Audio Bus or Submix is not a virtual hardware device that Spatial Root can select as an output.

The preferred `ue-root` audio path is:

```text
Spatial Root rendered PCM
    ↓
Unreal procedural source / generator
    ↓
Unreal submix or audio bus routing
    ↓
Unreal master output
    ↓
Unreal-selected audio device
```

If Spatial Root cannot provide rendered PCM blocks, then UE audio routing cannot be completed without a future Spatial Root host-render API.

## UE Audio Spike Order

Do not start by wiring Spatial Root audio.

Use this order:

1. Build/open the Unreal project.
2. Compile `SpatialRootHost` as an empty runtime plugin.
3. Create or stub a UE procedural audio source/generator.
4. Make the source output a test tone or silence.
5. Route that source through Unreal's normal output.
6. Optionally route it to a named submix such as `SpatialRootSubmix`.
7. Confirm diagnostics can report audio path mode.
8. Only then replace the test buffer with Spatial Root-rendered PCM if the locked Spatial Root API exposes it.

This separates two problems:

```text
Problem A: Can Unreal output generated audio from the plugin?
Problem B: Can Spatial Root provide rendered PCM blocks to Unreal?
```

Both must be true for mixer-routed Spatial Root playback.

## UE Audio Entry Points to Investigate

The agent should investigate, in this order:

1. `USoundWaveProcedural`
2. `USynthComponent`
3. `FSoundGenerator` / synth generator path, if available in this Unreal version
4. submix routing after the source works
5. Audio Bus or Source Bus only after source/generator entry is understood

Do not begin with Audio Bus as the insertion mechanism.

### `USoundWaveProcedural`

Use this if it is the fastest path to queue or generate PCM. Investigate:

- `USoundWaveProcedural`
- `OnGeneratePCMAudio`
- `QueueAudio`
- sample-rate setup
- channel-count setup
- audio render thread behavior
- underrun behavior

### `USynthComponent` / `FSoundGenerator`

Use this if Unreal's current version prefers synth/generator components for procedurally generated runtime audio.

Investigate:

- how to create a component that starts/stops from C++;
- how to expose it to Blueprint or the subsystem;
- whether it can request/render multichannel output;
- whether it can route to a submix.

### Submix Routing

Once audio is inside Unreal, route it through a named submix if practical:

```text
SpatialRootSource / TestGenerator
    ↓
SpatialRootSubmix
    ↓
Master Submix
    ↓
Output device
```

The agent should document whether:

- the submix was created in content or C++;
- the source routes to it;
- output channel count is preserved;
- Unreal downmixes;
- output is only stereo;
- channel count is unknown.

## Multichannel Output Questions

The agent must not assume arbitrary multichannel output works.

Investigate and document:

- Unreal output device selected;
- Unreal sample rate;
- Unreal output channel count, if accessible;
- source/generator requested channel count;
- Spatial Root rendered channel count, if accessible;
- layout-required channel count, if accessible;
- channel mismatch behavior;
- downmixing/truncation behavior.

If channel counts do not match, diagnostics should say so plainly.

Acceptable first-pass behavior:

```text
If N Spatial Root channels are requested and Unreal exposes M output channels:
- render or test the minimum safe channel count;
- zero-fill missing channels if applicable;
- warn if truncating or downmixing;
- document exactly what happened.
```

## UE Audio Diagnostics

Expose these fields if possible:

- audio path mode
- UE sample rate
- UE output channel count
- generator sample rate
- generator channel count
- generated frames
- underrun count
- last audio callback time, if available
- Spatial Root required channel count
- Spatial Root rendered channel count
- current ADM/BW64 path
- current layout path
- last error
- last operation

Audio path mode should be one of:

```text
UE procedural source
Spatial Root owns device
stub/test generator only
failed/not connected
unknown
```

## UE Audio Stop Conditions

Stop and document findings if:

- Unreal cannot output a test tone or silence from a generated/procedural source;
- Unreal procedural audio cannot be compiled in the current project;
- Unreal output appears limited to stereo and no multichannel path is visible;
- Spatial Root has no render-buffer path and the task would require refactoring Spatial Root;
- the agent would need to alter locked Spatial Root APIs;
- the agent cannot determine how ADM/BW64 and layout loading map to `EngineSession`.

A clear stop report is useful. Do not hide blockers.

## Plugin Build Focus

The first plugin should be:

```text
Unreal/Plugins/SpatialRootHost/
```

The plugin should compile before Spatial Root is linked.

Minimum plugin startup behavior:

- declares a runtime module;
- logs startup/shutdown;
- exposes a minimal subsystem/component or bridge stub;
- does not require Spatial Root to be present until the integration step.

Recommended initial log categories:

```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootHost, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootAudio, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSpatialRootDiagnostics, Log, All);
```

## Minimal Bridge Surface

Keep the bridge small.

Suggested C++ surface:

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

If a method cannot be implemented, leave a clear stub and document why.

## Required UI

The first UI should include:

- ADM/BW64 path field
- layout JSON path field
- Start
- Pause
- Resume
- Stop
- master gain dB
- DBAP focus
- speaker mix
- sub mix
- Live Update toggle
- Apply Params button
- diagnostics window

Do not implement object visualization in the first milestone.

## Required Task Log Update

At the end of each work session, update `docs/TASK_LOG.md`.

Include:

```markdown
## Local Unreal Setup

- Unreal path checked:
- Exists:
- Editor launch path:
- Project path:
- Build command:
- Build result:
- Plugin status:
- Spatial Root dependency status:
- Audio test status:
- Next step:

## Local Test Data

- sourceData path:
- Exists:
- ADM/BW64 candidates:
- Layout JSON candidates:
- Selected first ADM/BW64 test file:
- Selected first layout file:
- Open questions:

## UE Audio Investigation

- Audio entry point tested:
- Test tone/silence status:
- Submix routing status:
- UE sample rate:
- UE output channel count:
- Spatial Root render-buffer path:
- Blocker:
- Next step:
```

If `docs/TASK_LOG.md` does not exist, create it.

## Do Not Do

Do not:

- search for unrelated Unreal installs until `/Users/lucian/UE_5.7` has been checked;
- ignore `/Users/lucian/projects/spatialroot/sourceData`;
- copy large audio data into the repo without approval;
- target Windows;
- target Linux;
- create release packages;
- sign or notarize;
- add marketplace dependencies;
- implement visualization;
- build OSC control mode;
- launch an external Spatial Root executable;
- refactor Spatial Root;
- claim mixer-native integration without actual audio entering Unreal's audio path.

## Recommended `AGENTS.md` Link

Add this line to `AGENTS.md`:

```markdown
Before local Unreal implementation work, read `docs/UE_ROOT_AGENT_SKILL.md`. Use `/Users/lucian/UE_5.7` as the preferred Unreal Engine path and `/Users/lucian/projects/spatialroot/sourceData` as the preferred local test-data path. Document any deviation in `docs/TASK_LOG.md`.
```

## Official Unreal References for Agents

Agents should prefer official Unreal documentation when checking APIs.

Useful starting points:

- Audio Engine Overview: https://dev.epicgames.com/documentation/unreal-engine/audio-engine-overview-in-unreal-engine
- Audio Mixer Overview: https://dev.epicgames.com/documentation/unreal-engine/audio-mixer-overview-in-unreal-engine
- Submixes Overview: https://dev.epicgames.com/documentation/unreal-engine/overview-of-submixes-in-unreal-engine
- `USoundWaveProcedural`: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/USoundWaveProcedural
- `USoundWave::GeneratePCMData`: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/USoundWave/GeneratePCMData
- Plugins: https://dev.epicgames.com/documentation/unreal-engine/plugins-in-unreal-engine
- Integrating Third-Party Libraries: https://dev.epicgames.com/documentation/unreal-engine/integrating-third-party-libraries-into-unreal-engine
