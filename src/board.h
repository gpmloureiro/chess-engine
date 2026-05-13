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

class Board
{
public:
    uint64_t pieces[2][6];
    int promotionSq = -1;
    int promotionColor = -1;
    bool pendingPromotion = false;

    void init();
    bool makeMove(int initial, int final, int pieceType, int color);
    uint64_t getMoves(int sq, int pieceType, int color);
    bool isSquareAttacked(int sq, int attackerColor);
    bool isInCheck(int color);
    bool isCheckMate(int color);
    bool isStaleMate(int color);
    void updateOccupancies();
    int getPieceAt(int sq, int color);
    void applyPromotion(int pieceType);

private:
    static constexpr uint8_t WK_CASTLE = 0b00000001;
    static constexpr uint8_t WQ_CASTLE = 0b00000010;
    static constexpr uint8_t BK_CASTLE = 0b00000100;
    static constexpr uint8_t BQ_CASTLE = 0b00001000;

    uint64_t occupancy[2];
    uint64_t combinedOccupancy;

    uint8_t castlingRights;
    int enPassantSq;

    uint64_t handleKingMoves(uint64_t potentialMoves, int color);
    void updateEnPassant(int sq, int color);
    void updateCastlingRights(int sq);
    bool isLegalMove(int initial, int final, int pieceType, int color);
    bool isEnPassant(int sq, int pieceType);
    bool hasLegalMoves(int color);
};