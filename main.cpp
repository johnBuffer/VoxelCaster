#include <iostream>
#include <SFML/Graphics.hpp>

#include "DynamicBlur.h"

int main()
{
    int WIN_WIDTH = 500;
    int WIN_HEIGHT = 500;

    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "BLUR", sf::Style::Default);
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(true);
    window.setKeyRepeatEnabled(true);

    bool toggle = false;

    sf::RenderTexture finalRender;
    finalRender.create(WIN_WIDTH, WIN_HEIGHT);

    int BLUR_SCALE = 4;
    sf::RenderTexture blurTexture;
    blurTexture.create(WIN_WIDTH, WIN_HEIGHT);
    sf::RenderTexture downsizedTexture;
    downsizedTexture.create(WIN_WIDTH/BLUR_SCALE, WIN_HEIGHT/BLUR_SCALE);

    sf::Shader drawer;
    if (!drawer.loadFromFile("drawer.frag", sf::Shader::Fragment))
        std::cout << "Erreur" << std::endl;

    drawer.setParameter("WIDTH", WIN_WIDTH);
    drawer.setParameter("HEIGHT", WIN_HEIGHT);

    sf::Texture texture;
    texture.loadFromFile("img.jpg");

    DynamicBlur dblur(WIN_WIDTH, WIN_HEIGHT);

    double time = 0;

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
                case sf::Keyboard::A:
                    toggle = !toggle;
                    break;
                default:
                    break;
                }
            }
        }

        time += 0.1;

        window.clear(sf::Color::Black);

        finalRender.clear(sf::Color::Black);
        blurTexture.clear(sf::Color::Black);
        downsizedTexture.clear(sf::Color::Black);

        sf::RectangleShape rectangle(sf::Vector2f(100, 200));
        rectangle.setPosition(WIN_WIDTH/2, WIN_HEIGHT/2);
        rectangle.rotate(time);

        finalRender.draw(sf::Sprite(texture));
        finalRender.draw(rectangle);
        finalRender.display();

        //BLUR zone

        drawer.setParameter("MIN_HEIGHT", WIN_HEIGHT-mousePosition.y);
        drawer.setParameter("ORIGINAL_TEXTURE", finalRender.getTexture());
        finalRender.draw(sf::Sprite(dblur(finalRender.getTexture())), &drawer);
        finalRender.display();

        window.draw(sf::Sprite(finalRender.getTexture()));
        window.display();
    }

    return 0;
}
