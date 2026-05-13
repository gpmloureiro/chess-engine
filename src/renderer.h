#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "board.h"

class Renderer
{
public:
    Renderer(sf::RenderWindow &window);
    bool loadTextures();
    void draw(const Board &board);
    int getSquareFromMouse(int x, int y);
    void setHighlight(int selected, uint64_t legalMask);
    void setTurn(int turn);
    void setPromotionPending(bool pending, int color, int sq);
    int getPromotionChoice(int x, int y, int color);

private:
    sf::RenderWindow &window;
    sf::Texture pieceTextures[2][6];
    sf::RectangleShape square;
    sf::Font font;
    std::optional<sf::Text> turnText;

    int selectedSquare = -1;
    uint64_t legalMask = 0;
    int currentTurn = WHITE;

    bool promotionPending = false;
    int promotionColor = -1;
    int promotionSq = -1;

    static constexpr int TILE = 80;
    static const sf::Color LIGHT;
    static const sf::Color DARK;
    static const sf::Color SELECTED;
    static const sf::Color LEGAL;

    void drawPromotionPanel();
};