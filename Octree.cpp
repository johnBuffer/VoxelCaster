#include "Octree.h"
#include <iostream>
#include <string>
#include <bitset>

Octree::Octree() :
	m_max_scale(10),
	m_min_scale(1)
{
	m_elements.emplace_back();
	m_elements[0].subs = 1;
	for (int i(8); i--;)
		m_elements.emplace_back();

	std::cout << "Octree size " << m_elements.size() << std::endl;
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

	int last_index = -1;
	int last_sub;

	while (current_size >= (1 << m_min_scale))
	{
		OctreeElement& current_element = m_elements[current_index];
		current_size /= 2;

		int sub_x = current_x / current_size;
		int sub_y = current_y / current_size;
		int sub_z = current_z / current_size;

		int sub_index = sub_x + 2 * sub_y + 4 * sub_z;

		current_x -= current_size * sub_x;
		current_y -= current_size * sub_y;
		current_z -= current_size * sub_z;

		uint16_t current_mask = (current_element.mask << sub_index);
		
		if (!(current_mask & 0x8000))
		{
			// Index for new element's subs
			int new_index = m_elements.size();
			m_elements[current_element.subs + sub_index].subs = new_index;

			// Update current sub status
			current_element.mask |= (0x8000 >> sub_index);

			// Update parent's leaf status
			if (last_index != -1)
			{
				OctreeElement& last_element = m_elements[last_index];
				last_element.mask &= ~(0x80 >> last_sub);
			}

			// Allocate new subs if not leaf
			if (current_size >= (1 << m_min_scale))
			{
				for (int i(8); i--;)
				{
					m_elements.emplace_back();
				}
			}
		}

		last_index = current_index;
		last_sub = sub_index;

		int next_index = m_elements[current_index].subs + sub_index;
		current_index = next_index;
	}
}

void Octree::optimize()
{
	std::vector<OctreeElement> new_memory;

	optimize_element(m_elements[0], new_memory);

	m_elements = new_memory;
}

void Octree::print() const
{
	print_element(m_elements.front(), "");
	std::cout << std::endl;
}

int Octree::optimize_element(const OctreeElement& element, std::vector<OctreeElement>& new_memory)
{
	//std::cout << "Optimizing " << element.index << "..." << std::endl;
	return 0;
}

void Octree::print_element(const OctreeElement& element, const std::string& indent) const
{
	/*std::cout << indent << "ID: " << element.index << std::endl;
	for (int i(0); i < 8; ++i)
	{
		if (element.subs[i] != -1)
		{
			std::cout << indent + "  sub " << i << std::endl;
			print_element(m_elements[element.subs[i]], indent + "  ");
		}
	}*/
}
