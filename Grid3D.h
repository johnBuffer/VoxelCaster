#pragma once

#include <vector>

struct Point3D
{
	Point3D() :
		x(0), y(0), z(0) {}

	Point3D(float x_, float y_, float z_) :
		x(x_),
		y(y_),
		z(z_)
	{

	}

	float x, y, z;
};

struct HitPoint3D
{
	HitPoint3D() :
		m_point(), m_hit(false) {}

	HitPoint3D(float x, float y, float z, bool hit) :
		m_point(x, y, z), m_hit(hit) {}

	bool m_hit;
	Point3D m_point;
	Point3D m_text_coord;
};

class Grid3D
{
public:
	Grid3D(int x_size, int y_size, int z_size, int cell_size);

	std::vector<std::vector<int>>& operator[](int x);

	HitPoint3D castRay(const Point3D& start, const Point3D& ray_vector) const;

private:
	int m_cell_size;
	Point3D m_grid_size;
	std::vector<std::vector<std::vector<int>>> m_cells;
};
