#include "board.h"
#include "movegen.h"

using namespace MoveGen;

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

bool Board::makeMove(int initial, int final, int pieceType, int color)
{
    if (getMoves(initial, pieceType, color) & (1ULL << final))
    { // Check if move is in possible moves
        // Creates a 64-bit integer that contains two bits at 1, representing the initial and final position
        uint64_t moveMask = (1ULL << initial) | (1ULL << final);

        // Move position of the piece in its correspondent array
        pieces[color][pieceType] ^= moveMask;

        int enemy = !color;
        // Only enter the loop if the 'final' square is actually occupied by the enemy
        if (occupancy[enemy] & (1ULL << final))
            for (int p = 0; p < 6; p++)
                pieces[enemy][p] &= ~(1ULL << final);

        updateOccupancies();
        return true;
    }
    return false;
}

uint64_t Board::getMoves(int sq, int pieceType, int color)
{
    switch (pieceType)
    {
    case (PAWN):
        return pawnMoves(sq, color, combinedOccupancy, occupancy[!color]);
    case (KNIGHT):
        return knightMoves(sq) & ~occupancy[color];
    case (BISHOP):
        return bishopMoves(sq, combinedOccupancy) & ~occupancy[color];
    case (ROOK):
        return rookMoves(sq, combinedOccupancy) & ~occupancy[color];
    case (QUEEN):
        return queenMoves(sq, combinedOccupancy) & ~occupancy[color];
    case (KING):
        return kingMoves(sq) & ~occupancy[color];
    default:
        return 0;
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

int Board::getPieceAt(int sq, int color)
{
    for (int p = 0; p < 6; p++)
        if (pieces[color][p] & (1ULL << sq))
            return p;
    return -1;
}