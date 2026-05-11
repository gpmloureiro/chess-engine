#include "renderer.h"
#include <bit>

const sf::Color Renderer::LIGHT = sf::Color(240, 217, 181);
const sf::Color Renderer::DARK = sf::Color(181, 136, 99);
const sf::Color Renderer::SELECTED = sf::Color(100, 180, 100, 180);

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

// Returns which piece type is on sq for the given color, or -1
int Renderer::getPieceAt(const Board &board, int sq, int color)
{
    for (int p = 0; p < 6; p++)
        if (board.pieces[color][p] & (1ULL << sq))
            return p;
    return -1;
}

void Renderer::handleClick(int x, int y, Board &board)
{
    int sq = getSquareFromMouse(x, y);
    if (sq == -1)
        return;

    if (selectedSquare == -1)
    {
        // First click — select a piece if it belongs to current turn
        if (getPieceAt(board, sq, currentTurn) != -1)
            selectedSquare = sq;
    }
    else
    {
        // Second click — try to move
        int pieceType = getPieceAt(board, selectedSquare, currentTurn);

        if (pieceType != -1)
        {
            board.makeMove(selectedSquare, sq, pieceType, currentTurn);
            board.updateOccupancies();
            currentTurn ^= 1; // switch turn
        }

        selectedSquare = -1; // deselect regardless
    }
}

void Renderer::draw(const Board &board)
{
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
        }
    }

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