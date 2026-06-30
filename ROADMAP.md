# Roadmap

## v0.1 Local macOS Proof

1. Keep the Unreal project shell buildable.
2. Compile the `SpatialRootHost` plugin as a runtime plugin.
3. Prove Unreal can output generated audio from the plugin.
4. Link Spatial Root `EngineSessionCore` from the local checkout.
5. Call the Spatial Root lifecycle from `USpatialRootBridge`.
6. ✅ Spatial Root exposes `renderHostBlock()` via the Internal Host Bus API (`AudioOutputMode::InternalHostBus`) on the canonical `host-render` branch. See `internalDocs/HOST_RENDER_BACKEND.md` in `SpatialRoot/spatialroot`.
7. Feed `renderHostBlock()` output into Unreal audio via `USpatialRootRenderBusComponent` with `bUseSpatialRootHostBus` enabled.
8. ~~If render-buffer access does not exist, document the minimal future Spatial Root API.~~ (resolved — API exists)

## Deferred

- Visualization.
- OSC controller mode.
- External Spatial Root process launching.
- Windows and Linux targets.
- Marketplace dependencies.
- Production packaging.
- C ABI wrapper.
