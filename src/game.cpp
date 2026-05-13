#include "game.h"
#include <iostream>

Game::Game(Board &board, Renderer &renderer)
    : board(board), renderer(renderer) {}

// Called after a move (or promotion choice) is fully resolved
void Game::finishTurn()
{
    currentTurn ^= 1;
    renderer.setTurn(currentTurn);

    if (board.isCheckMate(currentTurn))
        std::cout << "Checkmate!\n";
    else if (board.isStaleMate(currentTurn))
        std::cout << "Stalemate!\n";
}

void Game::handleClick(int x, int y)
{
    // ── Promotion picker ────────────────────────────────────────────────────
    if (awaitingPromotion)
    {
        int choice = renderer.getPromotionChoice(x, y, board.promotionColor);
        if (choice != -1)
        {
            board.applyPromotion(choice);
            awaitingPromotion = false;
            renderer.setPromotionPending(false, -1, -1);
            finishTurn();
        }
        // Block all other board interaction until a piece is chosen
        return;
    }

    // ── Normal move handling ─────────────────────────────────────────────────
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
        // Second click — attempt move
        int pieceType = board.getPieceAt(selectedSquare, currentTurn);

        if (board.makeMove(selectedSquare, sq, pieceType, currentTurn))
        {
            if (board.pendingPromotion)
            {
                // Pawn reached the back rank — show picker, defer turn flip
                awaitingPromotion = true;
                renderer.setPromotionPending(true,
                                             board.promotionColor,
                                             board.promotionSq);
            }
            else
            {
                finishTurn();
            }
        }

        // Deselect regardless of whether the move succeeded
        selectedSquare = -1;
        legalMask = 0;
        renderer.setHighlight(-1, 0);
    }
}