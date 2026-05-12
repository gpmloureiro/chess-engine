#include "board.h"
#include "movegen.h"
#include <cstdlib>

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

    // bit 0 = WHITE kingside
    // bit 1 = WHITE queenside
    // bit 2 = BLACK kingside
    // bit 3 = BLACK queenside
    castlingRights = 0b00001111;
}

bool Board::makeMove(int initial, int final, int pieceType, int color)
{
    uint64_t possibleMoves = getMoves(initial, pieceType, color);

    if (possibleMoves & (1ULL << final))
    {
        int enemy = !color;
        int capturedPiece = -1;

        if (occupancy[enemy] & (1ULL << final))
            capturedPiece = getPieceAt(final, enemy);

        // Apply move
        pieces[color][pieceType] ^= (1ULL << initial) | (1ULL << final);
        if (capturedPiece != -1)
            pieces[enemy][capturedPiece] &= ~(1ULL << final);

        // Move rook if castling
        bool isCastle = (pieceType == KING && std::abs(final - initial) == 2);
        if (isCastle)
        {
            int rookFrom, rookTo;
            if (final > initial) // kingside
            {
                rookFrom = (color == WHITE) ? 7 : 63;
                rookTo = (color == WHITE) ? 5 : 61;
            }
            else // queenside
            {
                rookFrom = (color == WHITE) ? 0 : 56;
                rookTo = (color == WHITE) ? 3 : 59;
            }
            pieces[color][ROOK] ^= (1ULL << rookFrom) | (1ULL << rookTo);
        }

        updateOccupancies();

        // Verify legality
        if (isInCheck(color))
        {
            // Undo piece move
            pieces[color][pieceType] ^= (1ULL << initial) | (1ULL << final);
            if (capturedPiece != -1)
                pieces[enemy][capturedPiece] |= (1ULL << final);

            // Undo rook if castling
            if (isCastle)
            {
                int rookFrom, rookTo;
                if (final > initial)
                {
                    rookFrom = (color == WHITE) ? 7 : 63;
                    rookTo = (color == WHITE) ? 5 : 61;
                }
                else
                {
                    rookFrom = (color == WHITE) ? 0 : 56;
                    rookTo = (color == WHITE) ? 3 : 59;
                }
                pieces[color][ROOK] ^= (1ULL << rookFrom) | (1ULL << rookTo);
            }

            updateOccupancies();
            return false;
        }

        // Update castling rights
        if (pieceType == KING)
            castlingRights &= (color == WHITE) ? ~(WK_CASTLE | WQ_CASTLE) : ~(BK_CASTLE | BQ_CASTLE);
        if (pieceType == ROOK)
            updateCastlingRights(initial);
        if (capturedPiece == ROOK)
            updateCastlingRights(final);

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
        return handleKingMoves(kingMoves(sq) & ~occupancy[color], color);
    default:
        return 0;
    }
}

uint64_t Board::handleKingMoves(uint64_t potentialMoves, int color)
{
    uint64_t safeMoves = 0;

    while (potentialMoves)
    {
        // Get index of the next potential square
        int targetSq = __builtin_ctzll(potentialMoves);

        // Now isSquareAttacked is accessible because we are inside the Board scope
        if (!isSquareAttacked(targetSq, !color))
        {
            safeMoves |= (1ULL << targetSq);
        }

        // Clear the bit we just processed
        potentialMoves &= (potentialMoves - 1);
    }

    // Can't castle if in check
    if (isInCheck(color))
    {
        return safeMoves;
    }

    int kingSq = (color == WHITE) ? 4 : 60;

    // Handle castling king side
    if (castlingRights & (color == WHITE ? WK_CASTLE : BK_CASTLE))
    {
        int passSq = kingSq + 1;
        int landSq = kingSq + 2;

        if (!(combinedOccupancy & ((1ULL << passSq) | (1ULL << landSq))) && !isSquareAttacked(passSq, !color) && !isSquareAttacked(landSq, !color))
        {
            safeMoves |= (1ULL << landSq);
        }
    }

    // Handle castling queen side
    if (castlingRights & (color == WHITE ? WQ_CASTLE : BQ_CASTLE))
    {
        int passSq = kingSq - 1;
        int landSq = kingSq - 2;
        int rookPass = kingSq - 3;

        if (!(combinedOccupancy & ((1ULL << passSq) | (1ULL << landSq) | (1ULL << rookPass))) && !isSquareAttacked(passSq, !color) && !isSquareAttacked(landSq, !color))
        {
            safeMoves |= (1ULL << landSq);
        }
    }

    return safeMoves;
}

// Checks if there is a need to update the castling rights and applies them if needed
void Board::updateCastlingRights(int sq)
{
    switch (sq)
    {
    case 0:
        castlingRights &= ~WQ_CASTLE;
        break;
    case 7:
        castlingRights &= ~WK_CASTLE;
        break;
    case 56:
        castlingRights &= ~BQ_CASTLE;
        break;
    case 63:
        castlingRights &= ~BK_CASTLE;
        break;
    }
}

// Uses the Superpiece method
bool Board::isSquareAttacked(int sq, int enemyColor)
{
    if (MoveGen::pawnAttacks(sq, !enemyColor) & pieces[enemyColor][PAWN])
        return true;

    if (MoveGen::knightMoves(sq) & pieces[enemyColor][KNIGHT])
        return true;

    if (MoveGen::bishopMoves(sq, combinedOccupancy) &
        (pieces[enemyColor][BISHOP] | pieces[enemyColor][QUEEN]))
        return true;

    if (MoveGen::rookMoves(sq, combinedOccupancy) &
        (pieces[enemyColor][ROOK] | pieces[enemyColor][QUEEN]))
        return true;

    if (MoveGen::kingMoves(sq) & pieces[enemyColor][KING])
        return true;

    return false;
}

bool Board::isInCheck(int color)
{
    return isSquareAttacked(__builtin_ctzll(pieces[color][KING]), !color);
}

// Verifies if a position is legal by checking if it leeds to a check
bool Board::isLegalMove(int initial, int final, int pieceType, int color)
{
    // Save initial state
    uint64_t savedPieces[2][6];
    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            savedPieces[c][p] = pieces[c][p];
    uint64_t savedOcc[2] = {occupancy[WHITE], occupancy[BLACK]};
    uint64_t savedCombined = combinedOccupancy;

    // Simulate move
    pieces[color][pieceType] ^= (1ULL << initial) | (1ULL << final);
    pieces[!color][getPieceAt(final, !color)] &= ~(1ULL << final);
    updateOccupancies();

    bool result = !isInCheck(color);

    // Restore initial state
    for (int c = 0; c < 2; c++)
        for (int p = 0; p < 6; p++)
            pieces[c][p] = savedPieces[c][p];
    occupancy[WHITE] = savedOcc[WHITE];
    occupancy[BLACK] = savedOcc[BLACK];
    combinedOccupancy = savedCombined;

    return result;
}

// Checks if a given side has any legal moves
bool Board::hasLegalMoves(int color)
{
    for (int p = 0; p < 6; p++)
    {
        uint64_t bitboard = pieces[color][p];

        while (bitboard)
        {
            int sq = __builtin_ctzll(bitboard);
            uint64_t moves = getMoves(sq, p, color);

            while (moves)
            {
                int target = __builtin_ctzll(moves);

                if (isLegalMove(sq, target, p, color))
                {
                    return true;
                }
                moves &= moves - 1;
            }
            bitboard &= bitboard - 1;
        }
    }

    return false;
}

bool Board::isCheckMate(int color)
{
    if (!isInCheck(color))
    {
        return false;
    }

    return !hasLegalMoves(color);
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