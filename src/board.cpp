#include "board.h"

void Board::init()
{
    pieces[WHITE][PAWN] = 0x000000000000FF00ULL;
    pieces[WHITE][KNIGHT] = 0x0000000000000042ULL;
    pieces[WHITE][BISHOP] = 0x0000000000000024ULL;
    pieces[WHITE][ROOK] = 0x0000000000000081ULL;
    pieces[WHITE][QUEEN] = 0x0000000000000008ULL;
    pieces[WHITE][KING] = 0x0000000000000010ULL;

    pieces[BLACK][PAWN] = 0x00FF000000000000ULL;
    pieces[BLACK][KNIGHT] = 0x4200000000000000ULL;
    pieces[BLACK][BISHOP] = 0x2400000000000000ULL;
    pieces[BLACK][ROOK] = 0x8100000000000000ULL;
    pieces[BLACK][QUEEN] = 0x0800000000000000ULL;
    pieces[BLACK][KING] = 0x1000000000000000ULL;

    occupancy[WHITE] = 0x000000000000FFFFULL;
    occupancy[BLACK] = 0xFFFF000000000000ULL;
    combinedOccupancy = 0xFFFF00000000FFFFULL;
}

void Board::makeMove(int from, int to, int pieceType, int color)
{

    // Creates a 64-bit integer that contains two bits at 1, representing the initial and final position
    uint64_t moveMask = (1ULL << from) | (1ULL << to);

    // Move position of the piece in its correspondent array
    pieces[color][pieceType] ^= moveMask;

    int enemy = !color;
    // Only enter the loop if the 'to' square is actually occupied by the enemy
    if (occupancy[enemy] & (1ULL << to))
        for (int p = 0; p < 6; p++)
            pieces[enemy][p] &= ~(1ULL << to);

    updateOccupancies();
}

void Board::isLegalMove(int from, int to, int pieceType, int color)
{
    switch (pieceType)
    {
    case PAWN:
        if ((to == (from << to)))
            break;

    default:
        break;
    }
}

void Board::updateOccupancies()
{
    occupancy[WHITE] = occupancy[BLACK] = 0;

    for (int p = 0; p < 6; p++)
    {
        occupancy[WHITE] |= pieces[WHITE][p];
        occupancy[BLACK] |= pieces[BLACK][p];
    }

    combinedOccupancy = occupancy[WHITE] | occupancy[BLACK];
}