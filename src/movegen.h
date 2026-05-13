#pragma once
#include <cstdint>

namespace MoveGen
{
    uint64_t pawnAttacks(int sq, int color);
    uint64_t pawnMoves(int sq, int color, uint64_t combined, uint64_t enemy, int enPassantSq);
    uint64_t knightMoves(int sq);
    uint64_t bishopMoves(int sq, uint64_t combined);
    uint64_t rookMoves(int sq, uint64_t combined);
    uint64_t queenMoves(int sq, uint64_t combined);
    uint64_t kingMoves(int sq);
}