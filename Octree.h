#pragma once

#include <vector>

struct OctreeElement
{
	OctreeElement() :
		subs(-1),
		mask(0xFF)
	{}

	int32_t subs;
	uint16_t mask;

	//int32_t padding;
};

class Octree
{
public:
	Octree();

	const OctreeElement* getData() const;
	uint32_t getSize() const;

	void addElement(int x, int y, int z);
	void optimize();

	void print() const;

private:
	int m_max_scale;
	int m_min_scale;
	std::vector<OctreeElement> m_elements;

	int optimize_element(const OctreeElement& element, std::vector<OctreeElement>& new_memory);
	void print_element(const OctreeElement& element, const std::string& indent) const;
};