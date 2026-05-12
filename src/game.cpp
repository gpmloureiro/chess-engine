#include "game.h"
#include <iostream>

Game::Game(Board &board, Renderer &renderer)
    : board(board), renderer(renderer) {}

void Game::handleClick(int x, int y)
{
    int sq = renderer.getSquareFromMouse(x, y);
    if (sq == -1)
        return;

    if (selectedSquare == -1)
    {
        // First click — select own piece
        int pieceType = board.getPieceAt(sq, currentTurn);
        if (pieceType == -1)
            return;

        selectedSquare = sq;
        legalMask = board.getMoves(sq, pieceType, currentTurn);
        renderer.setHighlight(selectedSquare, legalMask);
    }
    else
    {
        // Second click — try to move
        int pieceType = board.getPieceAt(selectedSquare, currentTurn);

        // Check if move was valid
        if (board.makeMove(selectedSquare, sq, pieceType, currentTurn))
        {
            currentTurn ^= 1;
            renderer.setTurn(currentTurn);

            if (board.isCheckMate(currentTurn))
            {
                std::cout << "Checkmate!";
            }
        }

        selectedSquare = -1;
        legalMask = 0;
        renderer.setHighlight(-1, 0);
    }
}