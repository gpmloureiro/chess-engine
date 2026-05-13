#pragma once
#include "board.h"
#include "renderer.h"
#include "evaluation.h"

class Game
{
public:
    Game(Board &board, Renderer &renderer);
    void handleClick(int x, int y);

private:
    Board &board;
    Renderer &renderer;

    bool awaitingPromotion = false;
    int selectedSquare = -1;
    uint64_t legalMask = 0;
    int currentTurn = WHITE;

    void finishTurn();
};