# Spatial Root Agent Skill for `ue-root`

This document gives autonomous agents a focused Spatial Root orientation for the `ue-root` Unreal integration.

Use this together with:

- `DESIGN.md`
- `AGENTS.md`
- `docs/UE_ROOT_AGENT_SKILL.md`
- `docs/UNREAL_AGENT_SKILLS.md`, if present

This document is not a replacement for inspecting the current Spatial Root source. It provides expected concepts, likely search targets, and integration guardrails. The repository source is always the source of truth.

## Purpose

`ue-root` is an experimental Unreal Engine reference integration/template for embedding Spatial Root inside an Unreal application.

The Spatial Root side of the first milestone is to determine whether Unreal can drive an embedded Spatial Root runtime through the current locked API.

The critical questions are:

1. Can `EngineSession` be constructed and controlled from an external C++ host?
2. Can the current API load ADM/BW64 input directly, or does it require a LUSID package path first?
3. Can the current API load a loudspeaker layout JSON path directly?
4. Can runtime parameters be set directly?
5. Can playback be started, paused, resumed, and stopped cleanly?
6. Can Spatial Root provide rendered PCM buffers to Unreal without opening its own hardware audio device?
7. If not, what exact API is missing?

## Hard Constraints

- Spatial Root API is locked for this task.
- Do not refactor Spatial Root.
- Do not add a new audio backend unless explicitly instructed.
- Do not break the standalone Spatial Root app or CLI.
- Do not change existing Spatial Root behavior to make Unreal easier.
- Do not claim Unreal mixer routing works unless actual Spatial Root audio samples enter Unreal's audio path.
- Do not treat remembered architecture notes as source of truth. Verify current code.

## Local Spatial Root Paths

Likely local Spatial Root repository path:

```text
/Users/lucian/projects/spatialroot
```

Local test-data path:

```text
/Users/lucian/projects/spatialroot/sourceData
```

Agents should check these paths before guessing.

Required checks:

```bash
ls "/Users/lucian/projects/spatialroot"
ls "/Users/lucian/projects/spatialroot/sourceData"
cd "/Users/lucian/projects/spatialroot" && git status --short
cd "/Users/lucian/projects/spatialroot" && git branch --show-current
cd "/Users/lucian/projects/spatialroot" && git rev-parse --short HEAD
```

If the path does not exist, document that and stop before inventing another local dependency path.

## SourceData Discovery

Do not assume exact file names. Inspect `sourceData`.

Useful discovery commands:

```bash
find "/Users/lucian/projects/spatialroot/sourceData" -maxdepth 6 -type f
```

Narrow to likely integration assets:

```bash
find "/Users/lucian/projects/spatialroot/sourceData" -maxdepth 6 -type f \( \
  -iname "*.wav" -o \
  -iname "*.bw64" -o \
  -iname "*.bwf" -o \
  -iname "*.json" -o \
  -iname "*.lusid.json" \
\)
```

Classify discovered files as:

- ADM/BW64 candidate audio files
- ordinary WAV files
- layout JSON files
- LUSID scene/package files
- reports or metadata
- unknown files

Do not treat every `.wav` file as ADM/BW64. Verify through existing tools, file names, or code paths where possible.

## Expected Spatial Root Concepts

The current source should be inspected, but agents should expect to find concepts like:

- `EngineSession`
- `EngineOptions`
- `SceneInput`
- `LayoutInput`
- `RuntimeParams`
- staged setup/configuration
- scene or package loading
- layout application
- playback start
- pause/resume control
- runtime parameter updates
- backend shutdown
- streaming manager
- render bus
- output bus
- audio device configuration
- DBAP focus
- master gain
- speaker mix
- sub mix

If names differ in the current source, use the current source names and document the mapping.

## Expected EngineSession Lifecycle

The known conceptual lifecycle is staged:

```text
configure engine options
    ↓
load scene / input
    ↓
apply layout
    ↓
configure runtime params
    ↓
start
    ↓
pause/resume/runtime updates
    ↓
shutdown
```

The agent must verify the exact current method names and required ordering.

Do not call setup stages out of order if the source documents a different sequence.

## Required Source Searches

From the Spatial Root repo root, run searches equivalent to:

```bash
rg "class EngineSession|struct EngineSession|EngineSession" .
rg "configureEngine|loadScene|applyLayout|configureRuntime|RuntimeParams|EngineOptions|SceneInput|LayoutInput" .
rg "ADM|BW64|BWF|axml|chna|libbw64" .
rg "setPaused|pause|resume|start|shutdown|stop" .
rg "masterGain|gainDb|dbap|focus|speakerMix|subMix|lfe|RuntimeParams" .
rg "renderBlock|processBlock|offline|callback|AudioCallback|outputBuffer|interleaved|render bus|internal bus|device buffer" .
```

If `rg` is unavailable, use `grep -R`.

## EngineSession Investigation Checklist

Document answers in `docs/ENGINE_SESSION_API.md`.

Required questions:

- [ ] Where is `EngineSession` declared?
- [ ] Where is it used by the current app/CLI?
- [ ] Can it be constructed without GUI code?
- [ ] Can it be constructed without OSC/ParameterServer?
- [ ] What is the minimum setup sequence?
- [ ] What object or struct represents engine configuration?
- [ ] What object or struct represents scene input?
- [ ] What object or struct represents layout input?
- [ ] What object or struct represents runtime parameters?
- [ ] How are errors returned?
- [ ] Is `lastError` or equivalent available?
- [ ] How is shutdown performed?
- [ ] Are threads stopped during shutdown?
- [ ] Are any methods required to run on the main thread?
- [ ] Are any methods unsafe after audio starts?

## ADM/BW64 Investigation Checklist

The first `ue-root` pass wants ADM/BW64 input only.

Determine and document:

- [ ] Can `EngineSession` load ADM/BW64 directly?
- [ ] If yes, what method/config path does it use?
- [ ] If no, does Spatial Root require ADM/BW64 to be transcoded to LUSID first?
- [ ] Is the ADM/BW64 path tied to CULT Transcoder?
- [ ] Is direct ADM streaming supported?
- [ ] Is channel deinterleaving handled internally?
- [ ] Does the input path require 48 kHz?
- [ ] What errors occur for unsupported files?

Do not create a new ADM/BW64 ingest path during the first pass.

If ADM/BW64 requires a conversion step, document that clearly.

## Layout Investigation Checklist

Determine and document:

- [ ] What layout JSON schema/path is expected?
- [ ] How is `deviceChannel` interpreted?
- [ ] Does the layout define the required output channel count?
- [ ] Does the engine use compact internal render-bus indices?
- [ ] Does the engine map render bus channels to device channels?
- [ ] How are sparse/non-consecutive device channels handled?
- [ ] How are LFE/subwoofer channels represented?
- [ ] What happens if device channel count is lower than layout channel count?

This matters because Unreal output channel count may not match the Spatial Root layout.

## Runtime Parameter Checklist

Determine and document exact current ranges and defaults.

Expected parameters include:

- master gain in dB
- DBAP focus
- speaker mix
- sub mix
- pause/running state

Known expected conceptual ranges may include:

- master gain roughly `-60 dB` to `+12 dB`, default `0 dB`
- DBAP focus roughly `0.1` to `5.0`
- speaker mix and sub mix as scalar controls

These expected ranges must be verified against current source.

Do not hardcode old ranges if the source differs.

## Audio Architecture Checklist

The main `ue-root` technical question is whether Spatial Root can render into host-provided buffers.

Search for any path that can do this:

```text
Spatial Root scene/layout/runtime state
    ↓
render next block into float buffer
    ↓
host-provided interleaved or planar output
```

Possible names or concepts:

- offline render
- render block
- process block
- callback renderer
- audio callback
- internal render bus
- output bus
- backend abstraction
- device backend
- audio driver callback
- `onAudio`
- `AudioIO`
- `framesPerBuffer`
- interleaved output buffer
- per-channel output arrays

If found, document:

- exact function names;
- required state before calling;
- sample format;
- interleaved vs planar;
- number of frames per call;
- number of channels;
- ownership of buffers;
- thread assumptions;
- real-time safety;
- whether it opens a hardware device.

If no such path exists, document:

```text
No current host-pull render-buffer path found.
Unreal mixer routing requires a future Spatial Root render-buffer API.
```

Do not refactor the engine to add this during the first pass.

## Device-Owned Fallback

The preferred `ue-root` target is Unreal-owned output.

However, agents may document whether Spatial Root can run inside the Unreal process while owning its own device.

This is a fallback only.

Document as:

```text
Audio path mode: Spatial Root owns device
```

Do not present this as the same as Unreal mixer routing.

## Unreal Integration Surface

The Unreal bridge should call the smallest necessary part of Spatial Root.

Suggested Unreal bridge surface:

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

If a method cannot be implemented with current Spatial Root APIs, leave a stub and document why.

## Build Investigation

Agents should determine how Spatial Root currently builds.

From `/Users/lucian/projects/spatialroot`, inspect:

```bash
ls
find . -maxdepth 3 -iname "CMakeLists.txt" -o -iname "xmake.lua" -o -iname "Makefile"
find . -maxdepth 4 -type f \( -iname "*.a" -o -iname "*.dylib" -o -iname "*.so" \)
```

If CMake is used, inspect available options and targets.

Do not assume target names. Search:

```bash
rg "add_library|add_executable|spatialroot|EngineSession|option\(" CMakeLists.txt source cmake 2>/dev/null
```

If the engine only builds as an app/executable, document the work required to expose it as a library. Do not do a large build-system refactor unless explicitly asked.

## Threading and Shutdown Checklist

Embedding inside Unreal makes lifecycle and shutdown especially important.

Determine:

- [ ] What threads Spatial Root starts.
- [ ] Whether the streaming loader thread is started.
- [ ] Whether the audio callback thread is started.
- [ ] Whether the OSC/ParameterServer thread is started.
- [ ] Whether shutdown stops OSC first, audio backend second, streaming third, or another order.
- [ ] Whether `EngineSession` destructor is safe.
- [ ] Whether repeated initialize/start/stop cycles are supported.
- [ ] Whether pause is safer than stop.
- [ ] Whether hard seek/stop is unsupported.

Document exact findings.

## Diagnostics to Expose to Unreal

Expose or document availability of:

- engine initialized
- input path
- layout path
- input loaded
- layout loaded
- running/paused/stopped
- sample rate
- required render channel count
- output/device channel count
- current audio path mode
- xrun/underrun count
- invalid sample/NaN guard count, if available
- last error
- last operation

If a field is unavailable, include it in diagnostics as unavailable/unknown rather than hiding it.

## Documentation Outputs

Agents should update or create:

- `docs/ENGINE_SESSION_API.md`
- `docs/SPATIAL_ROOT_AUDIO_PATH.md`
- `docs/TASK_LOG.md`
- `docs/UNREAL_AUDIO_PATH.md`, if Unreal audio routing is touched

`docs/ENGINE_SESSION_API.md` should include:

```markdown
# EngineSession API Notes

## Source Files

## Construction

## Configuration Sequence

## ADM/BW64 Input

## Layout Input

## Runtime Parameters

## Playback and Transport

## Audio Output Modes

## Render-Buffer Availability

## Shutdown

## Missing API for Unreal Mixer Routing

## Open Questions
```

## Stop Conditions

Stop and document findings if:

- Spatial Root repo cannot be found.
- `EngineSession` cannot be located.
- `EngineSession` cannot be constructed from external C++.
- ADM/BW64 loading is not available through the current API.
- layout loading is not available through the current API.
- no render-buffer path exists and continuing would require refactoring Spatial Root.
- Spatial Root cannot build as a usable library or dependency for Unreal.
- current build system assumptions are unclear.
- changes would break standalone Spatial Root.

A stopped task with accurate notes is useful.

## Do Not Do

Do not:

- refactor Spatial Root;
- create a new public API;
- create a C ABI;
- add a new audio backend;
- change ADM/BW64 ingest behavior;
- change layout semantics;
- change DBAP behavior;
- change runtime parameter ranges;
- break standalone GUI/CLI behavior;
- commit large sourceData assets into `ue-root`;
- claim success without testing or evidence.

## End-of-Session Report

At the end of the session, write a concise summary to `docs/TASK_LOG.md`:

```markdown
## Spatial Root Investigation

- Spatial Root path:
- Branch:
- Commit:
- EngineSession found:
- EngineSession source files:
- ADM/BW64 load path:
- Layout load path:
- Runtime params path:
- Render-buffer path:
- Device-owned fallback:
- Build/link status:
- Main blocker:
- Next recommended step:
```

If a blocker is found, include evidence and the exact file/function names involved.
