# Procedural Planet Generation
## Current Features
- Implement 3D Perlin Noise
- Generate heightmaps with 3D Perlin Noise
- Generate an icosphere by subdividing an icosahedron
- Recolor icosphere based on height \[Removed for biome coloring\]
- Generate clouds
- Generate heatmap and moisture map to recolor based on 'biomes'
- Regenerate based off of rotation axis (new equator)
- Regenerate based off of sea level
## Planned Features
- Normal-based lighting rather than changing the color values
- Pseudo-volumetric water (Opacity changes with depth)
  - Eventually actual volumetric water/clouds
  - Atmospheric Scattering (Rayleigh)
- Biome Smoothing
- Trees
  - Blue noise with frequency varying with biome
- GUI
  - Render Text + Sliders
  - Text Input
- Capture image/video
- Save/Load Configuration
- Multiple Planets
- Civilization
  - Basic roads
  - Basic buildings
- Cache Parts of Planet Generation
- Better Input Handling
  - Holding Key
  - Camera Controls with Mouse
## Dependencies
- glm
- glfw3
- glew
## Getting Starting
To build it, clone the repo and use:
```bash
mkdir build
cd build
cmake ..
make -j8
```
To run it, use:
```bash
build/bin/procgen
```
## Controls
| Key   | Action                  |
| ----- | ----------------------- |
| 1     | Swap to Line Mode       |
| 2     | Swap to Fill Mode       |
| 3     | Render Planet Mode      |
| 4     | Render Heat Mode        |
| 5     | Render Moisture Mode    |
| Left  | Rotate Axis Left        |
| Right | Rotate Axis Right       |
| Up    | Increase # Subdivisions |
| Down  | Decrease # Subdivisions |
| R     | Generate New Seed       |
| W     | Increase Water Level    |
| S     | Decrease Water Level    |
| P     | Pause/Continue Rotation |
| Q     | Zoom Out                |
| E     | Zoom In                 |
## Report Link
[Link](https://quangmire.github.io/projects/procedural-planets)
