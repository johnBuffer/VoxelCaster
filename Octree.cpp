#include "Octree.h"
#include <iostream>

Octree::Octree() :
	m_max_scale(10),
	m_min_scale(4)
{
	m_elements.emplace_back(0);
}

const OctreeElement* Octree::getData() const
{
	return m_elements.data();
}

uint32_t Octree::getSize() const
{
	return m_elements.size();
}

void Octree::addElement(int x, int y, int z)
{
	int current_x = x;
	int current_y = y;
	int current_z = z;

	int current_size = 1 << m_max_scale;

	int current_index = 0;

	while (current_size >= (1 << m_min_scale))
	{
		current_size /= 2;

		int sub_x = current_x / current_size;
		int sub_y = current_y / current_size;
		int sub_z = current_z / current_size;

		int sub_index = sub_x + 2 * sub_y + 4 * sub_z;

		current_x -= current_size * sub_x;
		current_y -= current_size * sub_y;
		current_z -= current_size * sub_z;

		//std::cout << "Add voxel at sub " << sub_index << std::endl;
		// Check if current sub exists
		if (m_elements[current_index].subs[sub_index] == -1)
		{
			// If no create it
			int new_index = m_elements.size();
			m_elements[current_index].subs[sub_index] = new_index;
			m_elements.emplace_back(new_index);
		}

		OctreeElement& elem = m_elements[current_index];
		elem.is_leaf = 0;
		elem.is_empty = 0;

		current_index = elem.subs[sub_index];
	}

	m_elements[current_index].is_empty = 0;
}
