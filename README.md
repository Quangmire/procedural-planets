# Procedural Planet Generation
## Current Features
- Implement 3D Perlin Noise
- Generate heightmaps with 3D Perlin Noise
- Generate an icosphere by subdividing an icosahedron
- Recolor icosphere based on height
- Generate clouds
## Planned Features
- Generate heatmap and moisture map to recolor based on 'biomes'
- Normal-based lighting rather than changing the color values
- Pseudo-volumetric water
- Trees
- GUI
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
## Report LInk
[Currently Broken Link](https://quangmire.github.io/projects/procedural-planets)
