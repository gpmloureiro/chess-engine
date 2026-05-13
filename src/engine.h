#pragma once
#include "board.h"

struct BestMove
{
    int initial;
    int final;
    int pieceType;
    int score;
};

BestMove findBestMove(Board &board, int depth, int color);
int search(Board &board, int depth, int color);