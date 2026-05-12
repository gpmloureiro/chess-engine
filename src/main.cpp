#include <SFML/Graphics.hpp>
#include "board.h"
#include "renderer.h"
#include "game.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({640, 680}), "Chess Engine");
    window.setFramerateLimit(60);

    Board board;
    board.init();

    Renderer renderer(window);
    if (!renderer.loadTextures())
        return 1;

    Game game(board, renderer);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *click = event->getIf<sf::Event::MouseButtonPressed>())
                if (click->button == sf::Mouse::Button::Left)
                    game.handleClick(click->position.x, click->position.y);
        }

        window.clear();
        renderer.draw(board);
        window.display();
    }
}