#version 430

precision highp float;

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding=3) readonly buffer size
{
    int size_data[3];
};

layout(std430, binding=4) readonly buffer grid
{
    int grid_data[];
};

/*layout(std430, binding=5) readonly buffer position
{
    float position_data[3];
};*/

layout(rgba32f) uniform image2D img_output;

layout(location=2) uniform vec3 start_position;

layout(location=3) uniform vec2 camera_angles;

layout(location=4) uniform ivec2 render_dimension;

layout(location=5) uniform vec3 light_position;



struct HitPoint
{
	vec3 point;
	vec3 normal;
	int iteration;
	
	bool hit;
};

void castRay(in vec3 start, in vec3 ray, in int max_iter, out HitPoint result)
{
	// Initialization
	// We assume we have a ray vector:
	// vec = start + t*v
	
	ivec3 grid_size = ivec3(size_data[0], size_data[1], size_data[2]);
	
	float cell_size = 10.0;

	// cell_x and cell_y are the starting voxel's coordinates
	int cell_x = int(start.x / cell_size);
	int cell_y = int(start.y / cell_size);
	int cell_z = int(start.z / cell_size);

	// step_x and step_y describe if cell_x and cell_y
	// are incremented or decremented during iterations
	int step_x = ray.x<0 ? -1 : 1;
	int step_y = ray.y<0 ? -1 : 1;
	int step_z = ray.z<0 ? -1 : 1;

	// Compute the value of t for first intersection in x and y
	int dir_x = step_x > 0 ? 1 : 0;
	float t_max_x = ((cell_x + dir_x)*cell_size - start.x)/ray.x;

	int dir_y = step_y > 0 ? 1 : 0;
	float t_max_y = ((cell_y + dir_y)*cell_size - start.y) / ray.y;

	int dir_z = step_z > 0 ? 1 : 0;
	float t_max_z = ((cell_z + dir_z)*cell_size - start.z) / ray.z;
	
	// Compute how much (in units of t) we can move along the ray
	// before reaching the cell's width and height
	float t_dx = abs(float(cell_size) / ray.x);
	float t_dy = abs(float(cell_size) / ray.y);
	float t_dz = abs(float(cell_size) / ray.z);
	
	result.hit = false;
	
	int i=0;
	vec3 normal = vec3(1.0);
	while (i < max_iter && !result.hit)
	{
		i++;
		float t_max_min;
		if (t_max_x < t_max_y)
		{
			if (t_max_x < t_max_z)
			{
				t_max_min = t_max_x;
				t_max_x += t_dx;
				cell_x += step_x;
				
				normal = vec3(-step_x, 0.0, 0.0);
			}
			else
			{
				t_max_min = t_max_z;
				t_max_z += t_dz;
				cell_z += step_z;
				
				normal = vec3(0.0, 0.0, -step_z);
			}
		}
		else
		{
			if (t_max_y < t_max_z)
			{
				t_max_min = t_max_y;
				t_max_y += t_dy;
				cell_y += step_y;
				
				normal = vec3(0.0, -step_y, 0.0);
			}
			else
			{
				t_max_min = t_max_z;
				t_max_z += t_dz;
				cell_z += step_z;
				
				normal = vec3(0.0, 0.0, -step_z);
			}
		}

		if (cell_x >= 0 && cell_y >= 0 && cell_z >= 0 && cell_x < grid_size.x && cell_y < grid_size.y && cell_z < grid_size.z)
		{
			int grid_index = cell_x*(grid_size.y*grid_size.z) + cell_y*grid_size.z + cell_z;
			if (grid_data[grid_index] != 0)
			{
				result.point = start + t_max_min * ray;
				result.normal = normal;
				result.hit = true;
				result.iteration = i;
				break;
			}
		}
	}
}


void main() 
{  
	ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 screen_offset = ivec2(render_dimension.x/2, render_dimension.y/2);
	vec3 pixel = vec3(1.0);
	
	vec3 camera_origin = vec3(0, 0, -render_dimension.x);
	vec3 screen_coord = vec3(pixel_coord-screen_offset, 0.0);
	
	// Ray calculation
	float sin_a = sin(-camera_angles.x);
	float cos_a = cos(-camera_angles.x);
	float sin_a_y = sin(camera_angles.y);
	float cos_a_y = cos(camera_angles.y);
	
	vec3 ray = normalize(screen_coord - camera_origin);
	ray = vec3(ray.x, ray.y*cos_a_y-ray.z*sin_a_y, ray.y*sin_a_y+ray.z*cos_a_y);
	ray = vec3(ray.x*cos_a-ray.z*sin_a, ray.y, ray.x*sin_a+ray.z*cos_a);
		
	HitPoint result;
	
	ivec3 grid_size = ivec3(size_data[0], size_data[1], size_data[2]);
	
	castRay(start_position, ray, 200, result);
	
	if (result.hit)
	{	
		vec3 point_to_light = light_position - result.point;
		vec3 light_ray = normalize(point_to_light);
		
		HitPoint light_point;
		castRay(result.point+result.normal*0.01, light_ray, 50, light_point);
		
		float intensity = clamp(dot(result.normal, light_ray), 0.2, 1.0);
		pixel = abs(result.normal)*intensity;
		
		if (light_point.hit)
		{
			pixel *= 0.4;
		}
		else
		{			
		}
		
		pixel += vec3(result.iteration/200.0);
	}	
	
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coord, vec4(pixel, 1.0));
}