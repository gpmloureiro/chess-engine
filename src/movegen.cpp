#include "movegen.h"
#include "board.h"

static constexpr uint64_t NOT_A_COL = 0xFEFEFEFEFEFEFEFEULL;
static constexpr uint64_t NOT_H_COL = 0x7F7F7F7F7F7F7F7FULL;
static constexpr uint64_t NOT_B_COL = 0xFDFDFDFDFDFDFDFDULL;
static constexpr uint64_t NOT_G_COL = 0xBFBFBFBFBFBFBFBFULL;

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

        moves |= (pos << 9) & enemy & NOT_H_COL;
        moves |= (pos << 7) & enemy & NOT_A_COL;
    }
    else
    {
        if (sq / 8 == 6)
        {
            moves |= (pos >> 16) & ~combined & ~(combined >> 8);
        }
        moves |= (pos >> 8) & ~combined;

        moves |= (pos >> 9) & enemy & NOT_A_COL;
        moves |= (pos >> 7) & enemy & NOT_H_COL;
    }

    return moves;
}
