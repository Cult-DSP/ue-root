# Roadmap

## v0.1 Local macOS Proof

1. Keep the Unreal project shell buildable.
2. Compile the `SpatialRootHost` plugin as a runtime plugin.
3. Prove Unreal can output generated audio from the plugin.
4. Link Spatial Root `EngineSessionCore` from the local checkout.
5. Call the Spatial Root lifecycle from `USpatialRootBridge`.
6. ✅ Spatial Root exposes `renderHostBlock()` via the Internal Host Bus API (`AudioOutputMode::InternalHostBus`) on the canonical `host-render` branch. See `internalDocs/HOST_RENDER_BACKEND.md` in `SpatialRoot/spatialroot`.
7. ✅ Feed `renderHostBlock()` output into Unreal audio via `USpatialRootRenderBusComponent` with `bUseSpatialRootHostBus` enabled.
8. ✅ Render-buffer API question resolved; no minimal future API proposal is needed for the first host-bus pass.
9. ✅ Provide a source-controlled runtime harness and control UI through `ASpatialRootHostTestActor`, `AUERootGameMode`, and `UUERootControlPanel`.
10. Validate `Start Tone` audibly in an interactive editor/PIE run.
11. Validate Spatial Root host-bus playback audibly in an interactive editor/PIE run and document channel/sample-rate behavior.

## Deferred

- Visualization.
- OSC controller mode.
- External Spatial Root process launching.
- Windows and Linux targets.
- Marketplace dependencies.
- Production packaging.
- C ABI wrapper.
