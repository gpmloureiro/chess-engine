#include "movegen.h"
#include "board.h"

static constexpr uint64_t NOT_A_COL = 0xFEFEFEFEFEFEFEFEULL;
static constexpr uint64_t NOT_H_COL = 0x7F7F7F7F7F7F7F7FULL;
static constexpr uint64_t NOT_B_COL = 0xFDFDFDFDFDFDFDFDULL;
static constexpr uint64_t NOT_G_COL = 0xBFBFBFBFBFBFBFBFULL;

uint64_t MoveGen::pawnAttacks(int sq, int color)
{
    uint64_t pos = 1ULL << sq;
    uint64_t attacks = 0;

    if (color == WHITE)
    {
        attacks |= (pos << 7) & NOT_H_COL;
        attacks |= (pos << 9) & NOT_A_COL;
    }
    else
    {
        attacks |= (pos >> 7) & NOT_A_COL;
        attacks |= (pos >> 9) & NOT_H_COL;
    }
    return attacks;
}

uint64_t MoveGen::pawnMoves(int sq, int color, uint64_t combined, uint64_t enemy)
{
    uint64_t pos = 1ULL << sq;
    uint64_t moves = 0;

    if (color == WHITE)
    {
        if (sq / 8 == 1)
        {
            moves |= (pos << 16) & ~combined & ~(combined << 8);
        }
        moves |= (pos << 8) & ~combined;

        moves |= (pos << 9) & enemy & NOT_A_COL;
        moves |= (pos << 7) & enemy & NOT_H_COL;
    }
    else
    {
        if (sq / 8 == 6)
        {
            moves |= (pos >> 16) & ~combined & ~(combined >> 8);
        }
        moves |= (pos >> 8) & ~combined;

        moves |= (pos >> 9) & enemy & NOT_H_COL;
        moves |= (pos >> 7) & enemy & NOT_A_COL;
    }

    return moves;
}

uint64_t MoveGen::knightMoves(int sq)
{
    uint64_t pos = 1ULL << sq;
    uint64_t moves = 0;

    moves |= (pos << 17) & NOT_A_COL;
    moves |= (pos << 15) & NOT_H_COL;
    moves |= (pos << 10) & NOT_A_COL & NOT_B_COL;
    moves |= (pos << 6) & NOT_H_COL & NOT_G_COL;
    moves |= (pos >> 17) & NOT_H_COL;
    moves |= (pos >> 15) & NOT_A_COL;
    moves |= (pos >> 10) & NOT_H_COL & NOT_H_COL;
    moves |= (pos >> 6) & NOT_A_COL & NOT_B_COL;

    return moves;
}

static uint64_t rayAttacks(int sq, uint64_t combined, int dcol, int drow)
{
    uint64_t moves = 0;
    int col = sq % 8;
    int row = sq / 8;

    for (int i = 1; i < 8; i++)
    {
        int c = col + dcol * i;
        int r = row + drow * i;
        if (c < 0 || c > 7 || r < 0 || r > 7)
            break;

        int target = r * 8 + c;
        moves |= 1ULL << target;

        if (combined & (1ULL << target))
        {
            break;
        }
    }
    return moves;
}

uint64_t MoveGen::bishopMoves(int sq, uint64_t combined)
{
    return rayAttacks(sq, combined, 1, 1) | rayAttacks(sq, combined, -1, 1) | rayAttacks(sq, combined, 1, -1) | rayAttacks(sq, combined, -1, -1);
}

uint64_t MoveGen::rookMoves(int sq, uint64_t combined)
{
    return rayAttacks(sq, combined, 0, 1) | rayAttacks(sq, combined, 0, -1) | rayAttacks(sq, combined, 1, 0) | rayAttacks(sq, combined, -1, 0);
}

uint64_t MoveGen::queenMoves(int sq, uint64_t combined)
{
    return bishopMoves(sq, combined) | rookMoves(sq, combined);
}

uint64_t MoveGen::kingMoves(int sq)
{
    uint64_t pos = 1ULL << sq;
    uint64_t moves = 0;

    moves |= (pos << 8) | (pos >> 8);
    moves |= (pos << 7) | (pos >> 1) | (pos >> 9) & NOT_H_COL;
    moves |= (pos << 9) | (pos << 1) | (pos >> 7) & NOT_A_COL;

    return moves;
}