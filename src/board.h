// Board.h
#pragma once
#include <cstdint>

enum Color
{
    WHITE = 0,
    BLACK = 1
};
enum Piece
{
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5
};

struct Board
{
    uint64_t pieces[2][6];

    uint64_t occupancy[2];
    uint64_t combinedOccupancy;

    void init();
    void makeMove(int from, int to, int pieceType, int color);
    void isLegalMove(int from, int to, int pieceType, int color);
    void updateOccupancies();
};