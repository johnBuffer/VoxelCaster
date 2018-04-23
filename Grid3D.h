#pragma once

#include <vector>
#include <glm/glm.hpp>

struct HitPoint3D
{
	HitPoint3D() :
		m_point(), m_hit(false) {}

	HitPoint3D(float x, float y, float z, bool hit) :
		m_point(x, y, z), m_hit(hit) {}

	bool m_hit;
	glm::vec3 m_point;
	glm::vec3 m_text_coord;
};

class Grid3D
{
public:
	Grid3D(int x_size, int y_size, int z_size, int cell_size);

	int& operator()(int x, int y, int z);
	int at(int x, int y, int z) const;

	HitPoint3D castRay(const glm::vec3& start, const glm::vec3& ray_vector) const;
	
	const int* data() const;

private:
	int m_cell_size;
	glm::vec3 m_grid_size;
	std::vector<int> m_cells;
};
