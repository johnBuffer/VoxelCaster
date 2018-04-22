#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Grid2D.h"
#include "Grid3D.h"
#include "utils.h"
#include <glm/gtx/rotate_vector.hpp>

int main()
{
    int WIN_WIDTH = 500;
    int WIN_HEIGHT = 500;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(true);

    Grid3D grid(40, 40, 40, 25);

    for (int i(4000); i--;)
    {
        grid[rand()%40][rand()%40][rand()%40] = 1;
    }

	Point3D start(10, 10, 10);

	sf::VertexArray cast_va(sf::Points, WIN_WIDTH*WIN_HEIGHT);

	float movement_speed = 5.0f;

	float camera_horizontal_angle = 0;
	float camera_vertical_angle = 0;

	Point3D camera_origin(WIN_WIDTH / 2, WIN_HEIGHT / 2, -WIN_WIDTH / 1);
	Point3D camera_vec(WIN_WIDTH / 2 - camera_origin.x, WIN_HEIGHT / 2 - camera_origin.y, -camera_origin.z);
	float ray_length = length(camera_vec);
	camera_vec.x /= ray_length;
	camera_vec.y /= ray_length;
	camera_vec.z /= ray_length;

	double time = 0.0;

    while (window.isOpen())
    {
		time += 0.002;
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		camera_horizontal_angle += (mousePosition.x - WIN_WIDTH / 2)*0.001f;
		camera_vertical_angle += (mousePosition.y - WIN_HEIGHT / 2)*0.001f;
		sf::Mouse::setPosition(sf::Vector2i(WIN_WIDTH / 2, WIN_HEIGHT / 2), window);

		Point3D movement_vec = getCameraRay(camera_vec, camera_horizontal_angle, camera_vertical_angle);

		//Point3D light_position(0, 0, 0);
		Point3D light_position(500+500*cos(time), 0, 500+500 * sin(time));

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
					start.x += movement_vec.x*movement_speed;;
					start.y += movement_vec.y*movement_speed;;
					start.z += movement_vec.z*movement_speed;
                    break;
                case sf::Keyboard::S:
					start.x -= movement_vec.x*movement_speed;;
					start.y -= movement_vec.y*movement_speed;;
					start.z -= movement_vec.z*movement_speed;
                    break;
				case sf::Keyboard::Q:
					start.x -= movement_speed;
					break;
				case sf::Keyboard::D:
					start.x += movement_speed;
					break;
				case sf::Keyboard::A:
					start.z += movement_speed;
					break;
				case sf::Keyboard::E:
					start.z -= movement_speed;
					break;
                default:
                    break;
                }
            }
        }

		for (int x(0); x < WIN_WIDTH; ++x)
		{
			for (int y(0); y < WIN_HEIGHT; ++y)
			{
				Point3D ray(x - camera_origin.x, y - camera_origin.y, -camera_origin.z);
				float ray_length = sqrt(ray.x*ray.x + ray.y*ray.y + ray.z*ray.z);
				ray.x /= ray_length;
				ray.y /= ray_length;
				ray.z /= ray_length;

				HitPoint3D point = grid.castRay(start, getCameraRay(ray, camera_horizontal_angle, camera_vertical_angle));

				cast_va[x*WIN_HEIGHT + y].position = sf::Vector2f(x, y);
				if (point.m_hit)
				{
					Point3D hit_light_vec = directionnalVector(point.m_point, light_position);
					Point3D start_light(point.m_point.x - hit_light_vec.x*0.01f, point.m_point.y - hit_light_vec.y*0.01f, point.m_point.z - hit_light_vec.z*0.01f);

					HitPoint3D light_hit_point = grid.castRay(start_light, hit_light_vec);
					
					float light_intensity = 100000 / length(point.m_point - light_position);
					if (light_hit_point.m_hit)
						light_intensity *= 0.25f;

					float color = light_intensity;
					if (color > 255) color = 255;
					cast_va[x*WIN_HEIGHT + y].color = sf::Color(color, color, color);
				}
				else
				{
					cast_va[x*WIN_HEIGHT + y].color = sf::Color::Black;
				}
			}
		}
		
		window.clear(sf::Color::Black);

		window.draw(cast_va);

        window.display();
    }

    return 0;
}
