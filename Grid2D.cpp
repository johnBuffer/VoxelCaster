#include "Grid2D.h"

Grid2D::Grid2D(int width, int height, int cell_size) :
    grid_width(width),
    grid_height(height),
    cell_side_size(cell_size)
{
    cells.resize(width);
    for (auto& cell : cells)
    {
        cell.resize(height);
    }
}

void Grid2D::draw(sf::RenderTarget* target) const
{
    sf::VertexArray grix_va(sf::Lines, 2*(cells.size()+cells[0].size()));

    for (int x(0); x<grid_width; ++x)
    {
        grix_va[2*x+0].position = sf::Vector2f(x*cell_side_size, 0);
        grix_va[2*x+1].position = sf::Vector2f(x*cell_side_size, grid_height*cell_side_size);
    }

    for (int y(0); y<grid_height; ++y)
    {
        grix_va[2*grid_width+2*y+0].position = sf::Vector2f(0, y*cell_side_size);
        grix_va[2*grid_width+2*y+1].position = sf::Vector2f(grid_width*cell_side_size, y*cell_side_size);
    }

    for (int x(0); x<grid_width; ++x)
    {
        for (int y(0); y<grid_height; ++y)
        {
            if (cells[x][y])
            {
                sf::RectangleShape rect(sf::Vector2f(cell_side_size, cell_side_size));
                rect.setFillColor(sf::Color::Blue);
                rect.setPosition(x*cell_side_size, y*cell_side_size);

                target->draw(rect);
            }
        }
    }

    target->draw(grix_va);
}

int Grid2D::getCellSize() const
{
	return cell_side_size;
}

std::vector<int>& Grid2D::operator[](int i)
{
	return cells[i];
}

std::vector<Point2D> Grid2D::castRayAll(int start_x, int start_y, float vx, float vy) const
{
    /// Initialization
    /// We assume we have a ray vector:
    /// vec = start + t*v

    /// Empty vector for results
    std::vector<Point2D> result(0);

    /// cell_x and cell_y are the starting voxel's coordinates
    int cell_x = start_x/cell_side_size;
    int cell_y = start_y/cell_side_size;

    /// step_x and step_y describe if cell_x and cell_y
    /// are incremented or decremented during iterations
    int step_x = vx<0 ? -1 : 1;
    int step_y = vy<0 ? -1 : 1;

    /// Compute the value of t for first intersection in x and y
    float t_max_x=0;
    if (step_x > 0)
        t_max_x = (cell_x+1)*cell_side_size - start_x;
    else
        t_max_x = cell_x*cell_side_size - start_x;
    t_max_x /= vx;

    float t_max_y=0;
    if (step_y > 0)
        t_max_y = (cell_y+1)*cell_side_size - start_y;
    else
        t_max_y = cell_y*cell_side_size - start_y;
    t_max_y /= vy;
    //std::cout << step_y << " " << t_max_y << std::endl;

    /// Compute how much (in units of t) we can move along the ray
    /// before reaching the cell's width and height
    float t_dx = std::abs(float(cell_side_size) / vx);
    float t_dy = std::abs(float(cell_side_size) / vy);

    do
    {
        result.emplace_back(cell_x, cell_y);

        if(t_max_x < t_max_y)
        {
            t_max_x += t_dx;
            cell_x += step_x;
        }
        else
        {
            t_max_y += t_dy;
            cell_y += step_y;
        }
    }
    while (cell_x >= 0 && cell_y >= 0 && cell_x < grid_width && cell_y < grid_height);

    return result;
}

HitPoint2D Grid2D::castRayFirstHit(int start_x, int start_y, float vx, float vy) const
{
    /// Initialization
    /// We assume we have a ray vector:
    /// vec = start + t*v

    /// Empty vector for results
    HitPoint2D result;

    /// cell_x and cell_y are the starting voxel's coordinates
    int cell_x = start_x/cell_side_size;
    int cell_y = start_y/cell_side_size;

    /// step_x and step_y describe if cell_x and cell_y
    /// are incremented or decremented during iterations
    int step_x = vx<0 ? -1 : 1;
    int step_y = vy<0 ? -1 : 1;

    /// Compute the value of t for first intersection in x and y
    float t_max_x=0;
    if (step_x > 0)
        t_max_x = (cell_x+1)*cell_side_size - start_x;
    else
        t_max_x = cell_x*cell_side_size - start_x;
    t_max_x /= vx;

    float t_max_y=0;
    if (step_y > 0)
        t_max_y = (cell_y+1)*cell_side_size - start_y;
    else
        t_max_y = cell_y*cell_side_size - start_y;
    t_max_y /= vy;
    //std::cout << step_y << " " << t_max_y << std::endl;

    /// Compute how much (in units of t) we can move along the ray
    /// before reaching the cell's width and height
    float t_dx = std::abs(float(cell_side_size) / vx);
    float t_dy = std::abs(float(cell_side_size) / vy);

    while (cell_x >= 0 && cell_y >= 0 && cell_x < grid_width && cell_y < grid_height)
    {
        float t_max_min;
        if(t_max_x < t_max_y)
        {
            t_max_min = t_max_x;
            t_max_x += t_dx;
            cell_x += step_x;
        }
        else
        {
            t_max_min = t_max_y;
            t_max_y += t_dy;
            cell_y += step_y;
        }

        if (cell_x >= 0 && cell_y >= 0 && cell_x < grid_width && cell_y < grid_height)
        {
            if (cells[cell_x][cell_y])
            {
                result.point.x = start_x+t_max_min*vx;
                result.point.y = start_y+t_max_min*vy;
                result.has_hit = true;

                return result;
            }
        }
    }

    return result;
}
