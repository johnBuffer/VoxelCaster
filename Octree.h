#pragma once

#include <vector>

struct HitPoint3D
{
	glm::vec3 point;
	int iterations;
	bool m_hit;
};

struct OctreeElement
{
	OctreeElement() :
		index(-1),
		is_leaf(1),
		is_empty(1)
	{
		for (int i(8); i--;)
		{
			subs[i] = -1;
		}
	}

	OctreeElement(int id) :
		OctreeElement()
	{
		index = id;
	}

	int32_t index;
	int32_t is_leaf;
	int32_t is_empty;
	int32_t subs[8];

	//int32_t padding;
};

class Octree
{
public:
	Octree();

	const OctreeElement* getData() const;
	uint32_t getSize() const;

	void addElement(int x, int y, int z);

private:
	int m_max_scale;
	int m_min_scale;
	std::vector<OctreeElement> m_elements;
};