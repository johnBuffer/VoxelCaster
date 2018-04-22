#ifndef GRID2D_H_INCLUDED
#define GRID2D_H_INCLUDED

#include <SFML/Graphics.hpp>

struct Point2D
{
    Point2D() :
        x(0), y(0) {}

    Point2D(float x_, float y_) :
        x(x_),
        y(y_)
    {

    }

    float x, y;
};


struct HitPoint2D
{
    HitPoint2D() :
        point(0, 0), has_hit(false) {}

    HitPoint2D(float x, float y, bool hit) :
        point(x, y),
        has_hit(hit)
    {

    }

    bool has_hit;
    Point2D point;
};

class Grid2D
{
public:
    Grid2D(int width, int height, int cell_size=10);

    void draw(sf::RenderTarget* target) const;

    int getCellSize() const;

	std::vector<int>& operator[](int i);

	// Cast methods
    std::vector<Point2D> castRayAll(int start_x, int start_y, float vx, float vy) const;
    HitPoint2D castRayFirstHit(int start_x, int start_y, float vx, float vy) const;

private:
    int grid_width, grid_height;
    int cell_side_size;
    std::vector<std::vector<int>> cells;
};

#endif // GRID2D_H_INCLUDED
