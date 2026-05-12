#include "renderer.h"
#include <bit>

const sf::Color Renderer::LIGHT = sf::Color(240, 217, 181);
const sf::Color Renderer::DARK = sf::Color(181, 136, 99);
const sf::Color Renderer::SELECTED = sf::Color(100, 180, 100, 180);
const sf::Color Renderer::LEGAL = sf::Color(100, 100, 180, 150);

static const char *PIECE_FILES[2][6] = {
    {"src/assets/wP.png", "src/assets/wN.png", "src/assets/wB.png",
     "src/assets/wR.png", "src/assets/wQ.png", "src/assets/wK.png"},
    {"src/assets/bP.png", "src/assets/bN.png", "src/assets/bB.png",
     "src/assets/bR.png", "src/assets/bQ.png", "src/assets/bK.png"}};

Renderer::Renderer(sf::RenderWindow &window) : window(window)
{
    square.setSize({(float)TILE, (float)TILE});
}

bool Renderer::loadTextures()
{
    if (!font.openFromFile("src/assets/arial.TTF"))
        return false;

    turnText.emplace(font, "", 20);
    turnText->setPosition({8.f, 645.f});

    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            if (!pieceTextures[c][p].loadFromFile(PIECE_FILES[c][p]))
                return false;
    return true;
}

int Renderer::getSquareFromMouse(int x, int y)
{
    int col = x / TILE;
    int row = 7 - (y / TILE);
    if (col < 0 || col > 7 || row < 0 || row > 7)
        return -1;
    return row * 8 + col;
}

void Renderer::setHighlight(int selected, uint64_t mask)
{
    selectedSquare = selected;
    legalMask = mask;
}

void Renderer::setTurn(int turn)
{
    currentTurn = turn;
}

void Renderer::draw(const Board &board)
{
    // Draw squares
    for (int row = 7; row >= 0; row--)
    {
        for (int col = 0; col < 8; col++)
        {
            int sq = row * 8 + col;
            bool light = (row + col) % 2 != 0;
            square.setFillColor(light ? LIGHT : DARK);
            square.setPosition({(float)(col * TILE), (float)((7 - row) * TILE)});
            window.draw(square);

            // Highlight selected square
            if (sq == selectedSquare)
            {
                square.setFillColor(SELECTED);
                window.draw(square);
            }

            // Highlight legal moves
            if (legalMask & (1ULL << sq))
            {
                square.setFillColor(LEGAL);
                window.draw(square);
            }
        }
    }

    // Draw pieces
    for (int c = 0; c < 2; c++)
    {
        for (int p = 0; p < 6; p++)
        {
            uint64_t bb = board.pieces[c][p];
            while (bb)
            {
                int sq = __builtin_ctzll(bb);
                bb &= bb - 1;

                int col = sq % 8;
                int row = sq / 8;

                sf::Sprite sprite(pieceTextures[c][p]);
                auto size = pieceTextures[c][p].getSize();
                sprite.setScale({(float)TILE / size.x, (float)TILE / size.y});
                sprite.setPosition({(float)(col * TILE), (float)((7 - row) * TILE)});
                window.draw(sprite);
            }
        }
    }

    // Turn indicator
    if (currentTurn == WHITE)
    {
        turnText->setString("White's turn");
        turnText->setFillColor(sf::Color::White);
    }
    else
    {
        turnText->setString("Black's turn");
        turnText->setFillColor(sf::Color(180, 180, 180));
    }
    window.draw(*turnText);
}