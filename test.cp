#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding=3) readonly buffer size
{
    int grid_size[3];
};

layout(std430, binding=4) readonly buffer grid_data
{
    int grid[];
};

layout(rgba32f) uniform image2D img_output;

void main() 
{
  // base pixel colour for image
  
  uint current_x = gl_GlobalInvocationID.x;
  uint current_y = gl_GlobalInvocationID.y;
  
  int current_value = grid[current_x*grid_size[1] + current_y];
  
  vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
  
  if (current_value == 1)
  {
    pixel = vec4(1.0, 1.0, 1.0, 1.0);
  }
  
  // get index in global work group i.e x,y position
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
  
  //
  // interesting stuff happens here later
  //
  
  // output to a specific pixel in the image
  imageStore(img_output, pixel_coords, pixel);
}

