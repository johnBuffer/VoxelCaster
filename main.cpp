#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Grid2D.h"
#include "Grid3D.h"
#include "utils.h"
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>

int work_grp_size[3];

int main()
{
    int WIN_WIDTH = 1000;
    int WIN_HEIGHT = 1000;

	int RENDER_WIDTH = 500;
	int RENDER_HEIGHT = 500;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(true);

	int work_grp_cnt[3];

    Grid3D grid(40, 40, 40, 25);

    for (int i(4000); i--;)
    {
        grid[rand()%40][rand()%40][rand()%40] = 1;
    }

	Point3D start(10, 10, 10);

	sf::VertexArray cast_va(sf::Points, RENDER_WIDTH*RENDER_HEIGHT);

	for (int x(0); x < RENDER_WIDTH; ++x)
	{
		for (int y(0); y < RENDER_HEIGHT; ++y)
		{
			cast_va[x*RENDER_HEIGHT + y].position = sf::Vector2f(x, y);
		}
	}

	float movement_speed = 5.0f;

	float camera_horizontal_angle = 0;
	float camera_vertical_angle = 0;

	Point3D camera_origin(RENDER_WIDTH / 2, RENDER_HEIGHT / 2, -RENDER_WIDTH / 1);
	Point3D camera_vec(RENDER_WIDTH / 2 - camera_origin.x, RENDER_HEIGHT / 2 - camera_origin.y, -camera_origin.z);
	float ray_length = length(camera_vec);
	camera_vec.x /= ray_length;
	camera_vec.y /= ray_length;
	camera_vec.z /= ray_length;

	double time = 0.0;

	sf::RenderTexture render_texture;
	render_texture.create(RENDER_WIDTH, RENDER_HEIGHT);

	sf::Vector2i last_mouse_position;

    while (window.isOpen())
    {
		time += 0.002;
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

		camera_horizontal_angle += (mousePosition.x - WIN_WIDTH / 2)*0.001f;
		camera_vertical_angle += (mousePosition.y - WIN_HEIGHT / 2)*0.001f;
		sf::Mouse::setPosition(sf::Vector2i(WIN_WIDTH / 2, WIN_HEIGHT / 2), window);

		if (mousePosition != last_mouse_position)
		{
			render_texture.clear(sf::Color::Black);
		}

		last_mouse_position = mousePosition;

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

		for (int x(0); x < RENDER_WIDTH; ++x)
		{
			for (int y(0); y < RENDER_HEIGHT; ++y)
			{
				Point3D ray(x - camera_origin.x, y - camera_origin.y, -camera_origin.z);
				float ray_length = sqrt(ray.x*ray.x + ray.y*ray.y + ray.z*ray.z);
				ray.x /= ray_length;
				ray.y /= ray_length;
				ray.z /= ray_length;

				HitPoint3D point = grid.castRay(start, getCameraRay(ray, camera_horizontal_angle, camera_vertical_angle));

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
					cast_va[x*RENDER_HEIGHT + y].color = sf::Color(color, color, color);
				}
				else
				{
					cast_va[x*RENDER_HEIGHT + y].color = sf::Color::Black;
				}
			}
		}
		
		/*int ray_count = 50000;
		sf::VertexArray aprox_va(sf::Points, ray_count);
		for (int i(ray_count); i--;)
		{
			float x = rand() % RENDER_WIDTH;
			float y = rand() % RENDER_HEIGHT;

			aprox_va[i].position = sf::Vector2f(x, y);

			Point3D ray(x - camera_origin.x, y - camera_origin.y, -camera_origin.z);
			float ray_length = sqrt(ray.x*ray.x + ray.y*ray.y + ray.z*ray.z);
			ray.x /= ray_length;
			ray.y /= ray_length;
			ray.z /= ray_length;

			HitPoint3D point = grid.castRay(start, getCameraRay(ray, camera_horizontal_angle, camera_vertical_angle));

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
				aprox_va[i].color = sf::Color(color, color, color);
			}
			else
			{
				aprox_va[i].color = sf::Color::Black;
			}
		}

		render_texture.draw(aprox_va);*/
		
		render_texture.draw(cast_va);
		render_texture.display();

		sf::Sprite render_sprite(render_texture.getTexture());
		render_sprite.setScale(2.0f, 2.0f);
		
		window.clear(sf::Color::Black);

		window.draw(render_sprite);

        window.display();
    }

    return 0;
}
