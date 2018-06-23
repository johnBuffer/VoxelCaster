# VoxelCaster
Simple voxel ray caster in CPP / GLSL using compute shader and sparse voxel octree to store geometry. Currently it is not well optimized (~20fps for a 512 * 512 map with à GTX 1060)

Render image (Faces are colorized depending on their normal):
![Normal Render](https://github.com/johnBuffer/VoxelCaster/blob/master/render_1.png)

Depth Complexity (Whiter pixels request more computation time):
![Depth Complexity Render](https://github.com/johnBuffer/VoxelCaster/blob/master/depth_comp_1.png)
