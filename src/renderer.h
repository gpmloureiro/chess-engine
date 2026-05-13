#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "board.h"

class Renderer
{
public:
    Renderer(sf::RenderWindow &window);
    bool loadTextures();

    // Updated to include evaluation for the sidebar
    void draw(const Board &board, float evaluation = 0.0f);

    int getSquareFromMouse(int x, int y);
    void setHighlight(int selected, uint64_t legalMask);
    void setTurn(int turn);
    void setPromotionPending(bool pending, int color, int sq);
    int getPromotionChoice(int x, int y, int color);

private:
    sf::RenderWindow &window;
    sf::View view; // Handles the pixel-art scaling

    // Asset storage
    sf::Texture pieceTextures[2][6];
    sf::Texture boardTexture;
    std::optional<sf::Sprite> boardSprite;

    sf::RectangleShape square;
    sf::Font font;
    std::optional<sf::Text> turnText;

    // State management
    int selectedSquare = -1;
    uint64_t legalMask = 0;
    int currentTurn = 0; // Standardize to 0 (White) or 1 (Black)

    bool promotionPending = false;
    int promotionColor = -1;
    int promotionSq = -1;

    // Drawing helpers for the new UI layout
    void drawPromotionPanel();
    void drawEvalBar(float eval);
    void drawUI();

    // Visual Constants
    static const sf::Color LIGHT;
    static const sf::Color DARK;
    static const sf::Color SELECTED;
    static const sf::Color LEGAL;
};