# VoxelCaster
Simple voxel ray caster in C++ / GLSL using compute shader and sparse voxel octree to store geometry. Currently it is not well optimized (~20fps for a 512 * 512 map with a GTX 1060).

Pseudo Random noise is used for terrain generation (Fast Noise lib: https://github.com/Auburns/FastNoise)

Render image (Faces are colorized depending on their normal):
![Normal Render](https://github.com/johnBuffer/VoxelCaster/blob/master/render_1.png)

Depth Complexity (Whiter pixels request more computation time):
![Depth Complexity Render](https://github.com/johnBuffer/VoxelCaster/blob/master/depth_comp_1.png)
