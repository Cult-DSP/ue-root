# Example Content

Large audio assets are not committed to this repository.

Example ADM/Atmos material is available at:

https://huggingface.co/datasets/lucianparisi/atmos-data/tree/main

Preferred local Spatial Root test data path:

```text
/Users/lucian/projects/spatialroot/sourceData
```

Initial files discovered there include ADM/BW64 candidate WAV files, LUSID package folders, and LUSID scene JSON files.

Layout JSON files are available in the in-repo Spatial Root checkout under:

```text
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/speaker_layouts
```

Candidates include `stereo.json`, `example_layouts/5_1.json`, `example_layouts/quad_4.json`, and larger multichannel examples.

Current default candidate values in the C++ runtime harness are:

- ADM/BW64 candidate: `/Users/lucian/projects/spatialroot/sourceData/CANYON-ATMOS-LFE.wav`
- LUSID scene candidate: `/Users/lucian/projects/spatialroot/sourceData/lusid_package/scene.lusid.json`
- Layout JSON: `Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/source/spatial_engine/speaker_layouts/translab-sono-layout.json`

This candidate pair still needs interactive runtime confirmation before treating it as a known-good Spatial Root integration test. The local transcoder report files under `sourceData` do not confirm the pair; they record failed attempts to treat LUSID JSON as transcoder input.
