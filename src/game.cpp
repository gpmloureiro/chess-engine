#include "game.h"
#include "evaluation.h"
#include "engine.h"
#include <iostream>

Game::Game(Board &board, Renderer &renderer)
    : board(board), renderer(renderer) {}

void Game::finishTurn()
{
    currentTurn ^= 1;
    renderer.setTurn(currentTurn);

    // Check for game over
    if (board.isCheckMate(currentTurn))
    {
        std::cout << (currentTurn == WHITE ? "Black" : "White") << " wins!\n";
        return;
    }

    if (currentTurn == BLACK)
    {
        std::cout << "AI is thinking...\n";

        // Call recursive with depth 4
        BestMove aiMove = findBestMove(board, 4, BLACK);

        if (aiMove.initial != -1)
        {
            board.makeMove(aiMove.initial, aiMove.final, aiMove.pieceType, BLACK);
            if (board.pendingPromotion)
                board.applyPromotion(QUEEN);

            finishTurn();
        }

        std::cout << "Evaluation: " << evaluate(board.pieces) << std::endl;
    }
}

void Game::handleClick(int x, int y)
{
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
        return;
    }

    int sq = renderer.getSquareFromMouse(x, y);
    if (sq == -1)
        return;

    if (selectedSquare == -1)
    {
        int pieceType = board.getPieceAt(sq, currentTurn);
        if (pieceType == -1)
            return;

        selectedSquare = sq;
        legalMask = board.getMoves(sq, pieceType, currentTurn);
        renderer.setHighlight(selectedSquare, legalMask);
    }
    else
    {
        int pieceType = board.getPieceAt(selectedSquare, currentTurn);

        if (board.makeMove(selectedSquare, sq, pieceType, currentTurn))
        {
            if (board.pendingPromotion)
            {
                awaitingPromotion = true;
                selectedSquare = -1;
                legalMask = 0;

                renderer.setHighlight(-1, 0);
                renderer.setPromotionPending(true,
                                             board.promotionColor,
                                             board.promotionSq);
            }
            else
            {
                finishTurn();
            }
        }

        selectedSquare = -1;
        legalMask = 0;
        renderer.setHighlight(-1, 0);
    }
}