#pragma once

#include <cstdint>
#include "Bitboard.h"
#include "chess-types.h"
#include "Move.h"

using namespace std;
using U64 = uint64_t;

#define WHITE_PAWNS    bitboards[0]
#define WHITE_KNIGHTS  bitboards[1]
#define WHITE_BISHOPS  bitboards[2]
#define WHITE_ROOKS    bitboards[3]
#define WHITE_QUEENS   bitboards[4]
#define WHITE_KING     bitboards[5]

#define BLACK_PAWNS    bitboards[6]
#define BLACK_KNIGHTS  bitboards[7]
#define BLACK_BISHOPS  bitboards[8]
#define BLACK_ROOKS    bitboards[9]
#define BLACK_QUEENS   bitboards[10]
#define BLACK_KING     bitboards[11]



class Board {
public:
	Board();
	void print();

	Bitboard getOccupied() { return occupancy[BOTH_OCCUPIED]; }
	Bitboard getOccupancy(int idx) { return occupancy[idx]; };
	int getSideToMove() const { return sideToMove; };
	int  getEnPassantSquare() const { return enPassantSquare; }

    Bitboard getWhitePawns() { return WHITE_PAWNS; }
	Bitboard getWhiteKnights() { return WHITE_KNIGHTS; }
    Bitboard getWhiteBishops() { return WHITE_BISHOPS; }
	Bitboard getWhiteRooks() { return WHITE_ROOKS; }
	Bitboard getWhiteQueens() { return WHITE_QUEENS; }
	Bitboard getWhiteKing() { return WHITE_KING; }
    Bitboard getBlackPawns() { return BLACK_PAWNS; }
	Bitboard getBlackKnights() { return BLACK_KNIGHTS; }
	Bitboard getBlackBishops() { return BLACK_BISHOPS; }
	Bitboard getBlackRooks() { return BLACK_ROOKS; }
	Bitboard getBlackQueens() { return BLACK_QUEENS; }
	Bitboard getBlackKing() { return BLACK_KING; }

    const Bitboard& getOccupancy(int i) const { return occupancy[i]; }
	void updateOccupancy();
	bool makeMove(const Move& move);
	// void unmakeMove(const Move& move);

private:
	int pieceFinder(const int sq);

	Bitboard bitboards[12];
	Bitboard occupancy[3];

	// Turn determinator, games start on white
	int sideToMove = 0;
	int castlingRights = 15;
	int enPassantSquare = -1;
	int fiftyMoveCounter = 0;
	U64 zobristKey = 0ULL;	
};
