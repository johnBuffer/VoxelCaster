#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

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

struct HitPoint
{
    HitPoint() :
        point(0, 0), has_hit(false) {}

    HitPoint(float x, float y, bool hit) :
        point(x, y),
        has_hit(hit)
    {

    }

    bool has_hit;
    Point2D point;
};

struct Grid
{
    Grid(int width, int height, int cell_size=10) :
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

    void draw(sf::RenderTarget* target) const
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

    std::vector<Point2D> castRayAll(int start_x, int start_y, float vx, float vy)
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

    HitPoint castRayFirstHit(int start_x, int start_y, float vx, float vy)
    {
        /// Initialization
        /// We assume we have a ray vector:
        /// vec = start + t*v

        /// Empty vector for results
        HitPoint result;

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

    int grid_width, grid_height;
    int cell_side_size;
    std::vector<std::vector<int>> cells;
};


int main()
{
    int WIN_WIDTH = 1000;
    int WIN_HEIGHT = 500;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(true);
    window.setKeyRepeatEnabled(true);

    Grid grid(20, 20, 25);

    for (int i(100); i--;)
    {
        grid.cells[rand()%20][rand()%20] = 1;
    }

    int start_x = 10;
    int start_y = 10;

    std::vector<Point2D> last_result;

    while (window.isOpen())
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Z:
                    start_y-=5;
                    break;
                case sf::Keyboard::S:
                    start_y+=5;
                    break;
                case sf::Keyboard::Q:
                    start_x-=5;
                    break;
                case sf::Keyboard::D:
                    start_x+=5;
                    break;
                default:
                    break;
                }
            }
        }

        float vx_ray = mousePosition.x - start_x;
        float vy_ray = mousePosition.y - start_y;

        float length = sqrt(vx_ray*vx_ray + vy_ray*vy_ray);

        sf::CircleShape hit_circle(4);
        hit_circle.setFillColor(sf::Color::Red);
        hit_circle.setOrigin(2, 2);

        window.clear(sf::Color::Black);

        grid.draw(&window);

        float angle = acos(vx_ray/length);
        if (vy_ray<0) angle = -angle;
        float a_width = 3.1415926*0.3f;
        int da_count = 500;
        float da = a_width/float(da_count);

        for (int i(0); i<da_count; ++i)
        {
            float a = angle-a_width*0.5f+i*da;
            float vx = cos(a);
            float vy = sin(a);

            sf::VertexArray line_ray(sf::Lines, 2);
            line_ray[0].color = sf::Color::Green;
            line_ray[0].position = sf::Vector2f(start_x, start_y);
            line_ray[1].color = sf::Color::Green;

            HitPoint hit_point = grid.castRayFirstHit(start_x, start_y, vx, vy);
            if (hit_point.has_hit)
            {
                line_ray[1].position = sf::Vector2f(hit_point.point.x, hit_point.point.y);
                hit_circle.setPosition(hit_point.point.x, hit_point.point.y);

                float dist_vx = start_x - hit_point.point.x;
                float dist_vy = start_y - hit_point.point.y;
                float dist = sqrt(dist_vx*dist_vx + dist_vy*dist_vy)*cos(angle-a);

                sf::VertexArray line_wall(sf::Lines, 2);
                float wall_height = 2000/dist;
                float b = 10000/dist;
                if (b>255) b = 255;
                line_wall[0].color = sf::Color(0, 0, b);
                line_wall[0].position = sf::Vector2f(500+i, 100-0.2*wall_height);
                line_wall[1].color = sf::Color(0, 0, b);
                line_wall[1].position = sf::Vector2f(500+i, 100+0.8*wall_height);

                window.draw(line_wall);
            }
            else
                line_ray[1].position = sf::Vector2f(start_x+vx*1000, start_y+vy*1000);

            window.draw(line_ray);
            //window.draw(hit_circle);


        }

        window.display();
    }

    return 0;
}
