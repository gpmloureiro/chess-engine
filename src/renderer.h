#pragma once
#include <SFML/Graphics.hpp>
#include "board.h"
#include <optional>

class Renderer
{
public:
    Renderer(sf::RenderWindow &window);
    bool loadTextures();
    void draw(const Board &board);
    void handleClick(int x, int y, Board &board);

private:
    sf::RenderWindow &window;
    sf::Texture pieceTextures[2][6];
    sf::RectangleShape square;
    sf::Font font;
    std::optional<sf::Text> turnText;

    int selectedSquare = -1; // -1 means nothing selected
    int currentTurn = WHITE;

    static constexpr int TILE = 80;
    static const sf::Color LIGHT;
    static const sf::Color DARK;
    static const sf::Color SELECTED;

    int getSquareFromMouse(int x, int y);
    int getPieceAt(const Board &board, int sq, int color);
};