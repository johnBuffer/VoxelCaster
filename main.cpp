#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Grid2D.h"
#include "Grid3D.h"


int main()
{
    int WIN_WIDTH = 1000;
    int WIN_HEIGHT = 500;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(true);
    window.setKeyRepeatEnabled(true);

    Grid2D grid(20, 20, 25);

    for (int i(100); i--;)
    {
        grid[rand()%20][rand()%20] = 1;
    }

    int start_x = 10;
    int start_y = 10;

    std::vector<Point2D> last_result;

    while (window.isOpen())
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		float vx_ray = mousePosition.x - start_x;
		float vy_ray = mousePosition.y - start_y;
		float length = sqrt(vx_ray*vx_ray + vy_ray * vy_ray);
		vx_ray /= length;
		vy_ray /= length;

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
					start_x += vx_ray*5;
                    start_y += vy_ray*5;
                    break;
                case sf::Keyboard::S:
					start_x -= vx_ray * 5;
					start_y -= vy_ray * 5;
                    break;
                default:
                    break;
                }
            }
        }

        sf::CircleShape hit_circle(4);
        hit_circle.setFillColor(sf::Color::Red);
        hit_circle.setOrigin(2, 2);

        window.clear(sf::Color::Black);

        grid.draw(&window);

        float angle = acos(vx_ray);
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

            HitPoint2D hit_point = grid.castRayFirstHit(start_x, start_y, vx, vy);
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
