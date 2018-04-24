#version 430

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
	ivec3 cell_coord = ivec3(start / cell_size);

	// step_x and step_y describe if cell_x and cell_y
	// are incremented or decremented during iterations
	int step_x = ray.x<0 ? -1 : 1;
	int step_y = ray.y<0 ? -1 : 1;
	int step_z = ray.z<0 ? -1 : 1;
    
    ivec3 step = ivec3(step_x, step_y, step_z);

	// Compute the value of t for first intersection in x and y
	ivec3 dir = max(step, 0);
    vec3 t_max = ((cell_coord+dir)*cell_size - start) / ray;
	
	// Compute how much (in units of t) we can move along the ray
	// before reaching the cell's width and height
    vec3 t_delta = abs(vec3(cell_size) / ray);
	
	result.hit = false;
	
	int i=0;
	while (i < max_iter && !result.hit)
	{
		i++;
        int min_index = 2;
		/*if (t_max.x < t_max.y)
		{
			if (t_max.x < t_max.z)
			{
				min_index = 0;
                t_max.x += t_delta.x;
                cell_coord.x += step.x;
			}
			else
			{
				min_index = 2;
                t_max.z += t_delta.z;
                cell_coord.z += step.z;
			}
		}
		else
		{
			if (t_max.y < t_max.z)
			{
				min_index = 1;
                t_max.y += t_delta.y;
                cell_coord.y += step.y;
			}
			else
			{
				min_index = 2;
                t_max.z += t_delta.z;
                cell_coord.z += step.z;
			}
		}*/
        
        if (t_max.x < t_max.y && t_max.x < t_max.z)
        {
            min_index = 0;
            t_max.x += t_delta.x;
            cell_coord.x += step.x;
        }
        else if (t_max.y < t_max.x && t_max.y < t_max.z)
        {
            min_index = 1;
            t_max.y += t_delta.y;
            cell_coord.y += step.y;
        }
        else
        {
            t_max.z += t_delta.z;
            cell_coord.z += step.z;
        }
        
		if (cell_coord.x >= 0 && cell_coord.y >= 0 && cell_coord.z >= 0 && cell_coord.x < grid_size.x && cell_coord.y < grid_size.y && cell_coord.z < grid_size.z)
		{
			int grid_index = cell_coord.x*(grid_size.y*grid_size.z) + cell_coord.y*grid_size.z + cell_coord.z;
			if (grid_data[grid_index] != 0)
			{
				result.point = start + t_max[min_index] * ray;
				result.normal[min_index] = 1.0;
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
	vec4 pixel = vec4(vec3(1.0), 1.0);
	
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

	vec3 start = start_position;
	vec3 light_position = vec3(10000, 5000, 10000);
	
	HitPoint result;
	
	ivec3 grid_size = ivec3(size_data[0], size_data[1], size_data[2]);
	
	castRay(start, ray, 40, result);
    
    float cell_size = 10.0;
	
	if (result.hit)
	{   
        if (result.normal.x == 1.0)
        {
            vec2 tex_coord = fract(result.point.yz / cell_size);
            pixel = vec4(result.normal*length(tex_coord), 1.0);
		}
        else if (result.normal.y == 1.0)
        {
            vec2 tex_coord = fract(result.point.xz / cell_size);
            pixel = vec4(result.normal*length(tex_coord), 1.0);
		}
        else if (result.normal.z == 1.0)
        {
            vec2 tex_coord = fract(result.point.xy / cell_size);
            pixel = vec4(result.normal*length(tex_coord), 1.0);
		}
		/*vec3 point_to_light = light_position - result.point;
		vec3 light_ray = normalize(point_to_light);
		
		HitPoint light_point;
		castRay(result.point+result.normal*0.001, light_ray, 1, light_point);
		
		if (light_point.hit)
		{
			pixel = vec4(0.4*pixel.xyz, 1.0);
		}
		else
		{
			float intensity = clamp(dot(result.normal, light_ray), 0.7, 1.0);
			pixel = vec4(intensity*pixel.xyz, 1.0);
		}*/
		
		//pixel = vec4(pixel.xyz+vec3(result.iteration/200.0), 1.0);
	}
	
	//pixel = vec4(vec3(float(result.iteration) / 200.0), 1.0);
	
	
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coord, pixel);
}

