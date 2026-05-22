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
Unreal/Plugins/SpatialRootHost/Source/ThirdParty/SpatialRoot/spatialroot/spatial_engine/speaker_layouts
```

Candidates include `stereo.json`, `example_layouts/5_1.json`, `example_layouts/quad_4.json`, and larger multichannel examples.

A first ADM/BW64 plus LUSID scene plus layout JSON pair still needs confirmation before treating any run as a valid Spatial Root integration test.
