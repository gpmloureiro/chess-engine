#include "engine.h"
#include "evaluation.h"
#include <algorithm>

BestMove findBestMove(Board &board, int depth, int color)
{
    BestMove bestMove = {-1, -1, -1, (color == WHITE ? -100000 : 100000)};

    // Handle root of the tree
    for (int p = 0; p < 6; p++)
    {
        uint64_t bb = board.pieces[color][p];
        while (bb)
        {
            int initial = __builtin_ctzll(bb);
            uint64_t moves = board.getMoves(initial, p, color);
            while (moves)
            {
                int final = __builtin_ctzll(moves);

                int captured = board.getPieceAt(final, !color);
                int prevEP = board.enPassantSq;
                uint8_t prevCastle = board.castlingRights;
                bool wasEP = board.isEnPassant(final, p);
                bool wasCastle = (p == KING && std::abs(final - initial) == 2);

                if (board.makeMove(initial, final, p, color))
                {
                    if (board.pendingPromotion)
                        board.applyPromotion(QUEEN);

                    // Explore branch
                    int score = search(board, depth - 1, !color);

                    // Update if it's a better score
                    if (color == WHITE)
                    {
                        if (score > bestMove.score)
                        {
                            bestMove = {initial, final, p, score};
                        }
                    }
                    else
                    {
                        if (score < bestMove.score)
                        {
                            bestMove = {initial, final, p, score};
                        }
                    }
                    board.unmakeMove(initial, final, p, color, captured, prevEP, prevCastle, wasEP, wasCastle);
                }
                moves &= moves - 1;
            }
            bb &= bb - 1;
        }
    }
    return bestMove; // Now we actually have the coordinates to play!
}

// Implemented minimax algorithm
int search(Board &board, int depth, int color)
{
    if (depth == 0)
        return evaluate(board.pieces);

    // White looks to maximize the evaluation and black looks to minimize it
    int best = color == WHITE ? -9999 : 9999;
    bool hasMove = false;

    for (int p = 0; p < 6; p++)
    {
        uint64_t bb = board.pieces[color][p];

        while (bb)
        {
            int initial = __builtin_ctzll(bb);
            uint64_t moves = board.getMoves(initial, p, color);

            while (moves)
            {
                int final = __builtin_ctzll(moves);

                bool wasEP = board.isEnPassant(final, p);
                bool wasCastle = (p == KING && std::abs(final - initial) == 2);

                int captured = board.getPieceAt(final, !color);
                int prevEP = board.enPassantSq;
                uint8_t prevCastle = board.castlingRights;

                if (board.makeMove(initial, final, p, color))
                {
                    if (board.pendingPromotion)
                        board.applyPromotion(QUEEN); // Assume the queen promotion

                    hasMove = true;

                    // Simulate outcome if this was the move chosen
                    int score = search(board, depth - 1, !color);
                    best = color == WHITE ? std::max(best, score) : std::min(best, score);

                    board.unmakeMove(initial, final, p, color, captured, prevEP, prevCastle, wasEP, wasCastle);
                }
                moves &= moves - 1;
            }
            bb &= bb - 1;
        }
    }

    if (!hasMove)
        return board.isInCheck(color) ? (color == WHITE ? -99999 : 99999) : 0;

    return best;
}
