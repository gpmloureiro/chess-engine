#include "engine.h"
#include "evaluation.h"
#include <algorithm>

// Implemented minimax algorithm with alpha-beta pruning
int search(Board &board, int depth, int alpha, int beta, int color)
{
    if (depth == 0)
        return evaluate(board.pieces);

    // White looks to maximize the evaluation and black looks to minimize it
    int best = color == WHITE ? -1000000 : 1000000;
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
                    int score = search(board, depth - 1, alpha, beta, !color);

                    // MUST unmake move before potentially returning from a cutoff
                    board.unmakeMove(initial, final, p, color, captured, prevEP, prevCastle, wasEP, wasCastle);

                    if (color == WHITE)
                    {
                        best = std::max(best, score);
                        alpha = std::max(alpha, score);
                    }
                    else
                    {
                        best = std::min(best, score);
                        beta = std::min(beta, score);
                    }

                    // Alpha-Beta Pruning cutoff
                    if (beta <= alpha)
                        return best;
                }
                moves &= moves - 1;
            }
            bb &= bb - 1;
        }
    }

    if (!hasMove)
        return board.isInCheck(color) ? (color == WHITE ? -999999 : 999999) : 0;

    return best;
}

BestMove findBestMove(Board &board, int depth, int color)
{
    // Initialize alpha/beta with wide boundaries
    int alpha = -1000000;
    int beta = 1000000;
    BestMove bestMove = {-1, -1, -1, (color == WHITE ? -1000000 : 1000000)};

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

                    // Explore branch with alpha-beta window
                    int score = search(board, depth - 1, alpha, beta, !color);

                    // Update if it's a better score
                    if (color == WHITE)
                    {
                        if (score > bestMove.score)
                        {
                            bestMove = {initial, final, p, score};
                            alpha = std::max(alpha, score);
                        }
                    }
                    else
                    {
                        if (score < bestMove.score)
                        {
                            bestMove = {initial, final, p, score};
                            beta = std::min(beta, score);
                        }
                    }
                    board.unmakeMove(initial, final, p, color, captured, prevEP, prevCastle, wasEP, wasCastle);
                }
                moves &= moves - 1;
            }
            bb &= bb - 1;
        }
    }
    return bestMove;
}