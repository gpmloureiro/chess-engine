#include "renderer.h"
#include <bit>
#include <algorithm>

// Colors for highlights
const sf::Color Renderer::LIGHT = sf::Color(240, 217, 181);
const sf::Color Renderer::DARK = sf::Color(181, 136, 99);
const sf::Color Renderer::SELECTED = sf::Color(100, 180, 100, 180);
const sf::Color Renderer::LEGAL = sf::Color(100, 100, 180, 150);

static const sf::Color PROMO_BG = sf::Color(235, 235, 210, 245);
static const sf::Color PROMO_BORDER = sf::Color(80, 80, 80, 220);

// Promotion pieces configuration
static const int PROMO_PIECES[4] = {QUEEN, ROOK, BISHOP, KNIGHT};

static const char *PIECE_FILES[2][6] = {
    {"src/assets/wP.png", "src/assets/wN.png", "src/assets/wB.png",
     "src/assets/wR.png", "src/assets/wQ.png", "src/assets/wK.png"},
    {"src/assets/bP.png", "src/assets/bN.png", "src/assets/bB.png",
     "src/assets/bR.png", "src/assets/bQ.png", "src/assets/bK.png"}};

// Asset and Layout Constants
const float BOARD_X_OFF = 30.f;
const float PIECE_X_OFF = BOARD_X_OFF + 7.f;
const float PIECE_Y_OFF = 19.f;
const float TILE_W = 16.f;
const float TILE_H = 12.f;  // ADJUST THIS to match your perspective board's square height
const float PIECE_H = 32.f; // Your 16x32 piece assets

Renderer::Renderer(sf::RenderWindow &window) : window(window)
{
    // SFML 3.0: Explicitly setting the logical resolution view
    // (Eval Bar 20 + Board 128 + UI Padding)
    view.setSize({188.f, 140.f});
    view.setCenter({94.f, 70.f});

    // Square shape for selection/legal move highlights
    square.setSize({TILE_W, TILE_H});
}

bool Renderer::loadTextures()
{
    // Load Board PNG
    if (!boardTexture.loadFromFile("src/assets/board.png"))
        return false;
    boardTexture.setSmooth(false);

    boardSprite.emplace(boardTexture);
    boardSprite->setPosition({BOARD_X_OFF, 0.f});

    // Load Piece Textures
    for (int c = 0; c < 2; c++)
    {
        for (int p = 0; p < 6; p++)
        {
            if (!pieceTextures[c][p].loadFromFile(PIECE_FILES[c][p]))
                return false;
            pieceTextures[c][p].setSmooth(false); // Keeps pixel art sharp
        }
    }
    return true;
}

void Renderer::draw(const Board &board, float evaluation)
{
    window.setView(view);

    // 1. Draw the perspective board background
    if (boardSprite)
    {
        window.draw(*boardSprite);
    }

    // 2. Draw Highlights (Selected and Legal moves)
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int sq = row * 8 + col;
            float px = PIECE_X_OFF + (col * TILE_W);
            float py = (7 - row) * TILE_H + PIECE_Y_OFF + 1.f;

            if (sq == selectedSquare)
            {
                square.setFillColor(SELECTED);
                square.setPosition({px, py});
                window.draw(square);
            }
            if (legalMask & (1ULL << sq))
            {
                square.setFillColor(LEGAL);
                square.setPosition({px, py});
                window.draw(square);
            }
        }
    }

    // 3. Draw Pieces with 3D Offset with Z order depth
    for (int row = 7; row >= 0; row--)
    {
        for (int col = 0; col < 8; col++)
        {
            int sq = row * 8 + col;

            // Find if there is a piece on this square
            for (int c = 0; c < 2; c++)
            {
                for (int p = 0; p < 6; p++)
                {
                    if (board.pieces[c][p] & (1ULL << sq))
                    {
                        sf::Sprite sprite(pieceTextures[c][p]);

                        float px = PIECE_X_OFF + (col * TILE_W);
                        // Use your same offset logic
                        float py = ((7 - row) * TILE_H) + TILE_H - PIECE_H + PIECE_Y_OFF;

                        sprite.setPosition({px, py});
                        window.draw(sprite);
                    }
                }
            }
        }
    }

    // 4. UI Overlays
    if (promotionPending)
        drawPromotionPanel();
    drawEvalBar(evaluation);
    drawUI();
}

void Renderer::drawEvalBar(float eval)
{
    sf::RectangleShape bg({10.f, 111.f});
    bg.setPosition({10.f, 14.f});
    bg.setFillColor(sf::Color(40, 40, 40));
    window.draw(bg);

    float clamped = std::clamp(eval / 100, -5.f, 5.f);
    float t = (clamped + 5.f) / 10.f; // 0.0 to 1.0 range
    float h = 112.f * t;

    sf::RectangleShape whitePart({10.f, h});
    whitePart.setPosition({10.f, 111.f - h + 14.f});
    whitePart.setFillColor(sf::Color::White);
    window.draw(whitePart);
}

void Renderer::drawUI()
{
    float uiX = BOARD_X_OFF + (8 * TILE_W) + 10.f;
    sf::CircleShape turnMarker(4.f);
    turnMarker.setPosition({uiX, 10.f});

    if (currentTurn == 0)
    { // WHITE
        turnMarker.setFillColor(sf::Color::White);
    }
    else
    {
        turnMarker.setFillColor(sf::Color(60, 60, 60));
    }
    turnMarker.setOutlineThickness(1.f);
    turnMarker.setOutlineColor(sf::Color::White);
    window.draw(turnMarker);
}

void Renderer::drawPromotionPanel()
{
    int col = promotionSq % 8;
    sf::RectangleShape overlay({128.f, 112.f});
    overlay.setPosition({BOARD_X_OFF, 0.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    for (int i = 0; i < 4; i++)
    {
        int boardRow = (promotionColor == 0) ? (7 - i) : i;
        float px = BOARD_X_OFF + (col * TILE_W);
        float py = (7 - boardRow) * TILE_H;

        sf::RectangleShape bg({TILE_W, TILE_H});
        bg.setFillColor(PROMO_BG);
        bg.setOutlineColor(PROMO_BORDER);
        bg.setOutlineThickness(1.f);
        bg.setPosition({px, py});
        window.draw(bg);

        sf::Sprite sprite(pieceTextures[promotionColor][PROMO_PIECES[i]]);
        // Offset promotion piece just like board pieces
        sprite.setPosition({px, py + TILE_H - PIECE_H});
        window.draw(sprite);
    }
}

int Renderer::getSquareFromMouse(int x, int y)
{
    sf::Vector2f worldPos = window.mapPixelToCoords({(float)x, (float)y});

    int col = static_cast<int>((worldPos.x - PIECE_X_OFF) / TILE_W);
    int row = 7 - static_cast<int>((worldPos.y - PIECE_Y_OFF) / TILE_H);

    if (col < 0 || col > 7 || row < 0 || row > 7)
        return -1;
    return row * 8 + col;
}

int Renderer::getPromotionChoice(int x, int y, int color)
{
    if (!promotionPending)
        return -1;
    sf::Vector2f worldPos = window.mapPixelToCoords({(float)x, (float)y});

    int col = static_cast<int>((worldPos.x - BOARD_X_OFF) / TILE_W);
    if (col != promotionSq % 8)
        return -1;

    int boardRow = 7 - static_cast<int>(worldPos.y / TILE_H);
    if (color == 0)
    { // WHITE
        int idx = 7 - boardRow;
        if (idx >= 0 && idx <= 3)
            return PROMO_PIECES[idx];
    }
    else
    { // BLACK
        int idx = boardRow;
        if (idx >= 0 && idx <= 3)
            return PROMO_PIECES[idx];
    }
    return -1;
}

void Renderer::setHighlight(int selected, uint64_t mask)
{
    selectedSquare = selected;
    legalMask = mask;
}

void Renderer::setTurn(int turn) { currentTurn = turn; }

void Renderer::setPromotionPending(bool pending, int color, int sq)
{
    promotionPending = pending;
    promotionColor = color;
    promotionSq = sq;
}