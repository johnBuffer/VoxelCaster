#include "Grid3D.h"

Grid3D::Grid3D(int x_size, int y_size, int z_size, int cell_size) :
	m_cell_size(cell_size),
	m_grid_size(x_size, y_size, z_size)
{
	m_cells.resize(x_size);
	for (auto& vec : m_cells)
	{
		vec.resize(y_size);
		for (auto& sub_vec : vec)
		{
			sub_vec.resize(z_size);
		}
	}
}

HitPoint3D Grid3D::castRay(const Point3D& start, const Point3D& ray_vector) const
{
	// Initialization
	// We assume we have a ray vector:
	// vec = start + t*v

	// cell_x and cell_y are the starting voxel's coordinates
	int cell_x = start.x / m_cell_size;
	int cell_y = start.y / m_cell_size;
	int cell_z = start.z / m_cell_size;

	// step_x and step_y describe if cell_x and cell_y
	// are incremented or decremented during iterations
	int step_x = ray_vector.x<0 ? -1 : 1;
	int step_y = ray_vector.y<0 ? -1 : 1;
	int step_z = ray_vector.z<0 ? -1 : 1;

	// Compute the value of t for first intersection in x and y
	int dir_x = step_x > 0 ? 1 : 0;
	float t_max_x = ((cell_x + dir_x)*m_cell_size - start.x)/ray_vector.x;

	int dir_y = step_y > 0 ? 1 : 0;
	float t_max_y = ((cell_y + dir_y)*m_cell_size - start.y) / ray_vector.y;

	int dir_z = step_z > 0 ? 1 : 0;
	float t_max_z = ((cell_z + dir_z)*m_cell_size - start.z) / ray_vector.z;
	
	// Compute how much (in units of t) we can move along the ray
	// before reaching the cell's width and height
	float t_dx = std::abs(float(m_cell_size) / ray_vector.x);
	float t_dy = std::abs(float(m_cell_size) / ray_vector.y);
	float t_dz = std::abs(float(m_cell_size) / ray_vector.z);

	while (cell_x >= 0 && cell_y >= 0 && cell_z >= 0 && cell_x < m_grid_size.x && cell_y < m_grid_size.y && cell_z < m_grid_size.z)
	{
		float t_max_min;
		if (t_max_x < t_max_y)
		{
			if (t_max_x < t_max_z)
			{
				t_max_min = t_max_x;
				t_max_x += t_dx;
				cell_x += step_x;
			}
			else
			{
				t_max_min = t_max_z;
				t_max_z += t_dz;
				cell_z += step_z;
			}
		}
		else
		{
			if (t_max_y < t_max_z)
			{
				t_max_min = t_max_y;
				t_max_y += t_dy;
				cell_y += step_y;
			}
			else
			{
				t_max_min = t_max_z;
				t_max_z += t_dz;
				cell_z += step_z;
			}
		}

		if (cell_x >= 0 && cell_y >= 0 && cell_z >= 0 && cell_x < m_grid_size.x && cell_y < m_grid_size.y && cell_z < m_grid_size.z)
		{
			if (m_cells[cell_x][cell_y][cell_z])
			{
				float hit_x = start.x + t_max_min * ray_vector.x;
				float hit_y = start.y + t_max_min * ray_vector.y;
				float hit_z = start.z + t_max_min * ray_vector.z;

				return HitPoint3D(hit_x, hit_y, hit_z, true);
			}
		}
	}

	return HitPoint3D();
}
