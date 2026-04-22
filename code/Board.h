#pragma once

#include "Bitboard.h"
#include "Move.h"
#include "chess-types.h"
#include <cstdint>

using namespace std;
using U64 = uint64_t;

#define WHITE_PAWNS bitboards[0]
#define WHITE_KNIGHTS bitboards[1]
#define WHITE_BISHOPS bitboards[2]
#define WHITE_ROOKS bitboards[3]
#define WHITE_QUEENS bitboards[4]
#define WHITE_KING bitboards[5]

#define BLACK_PAWNS bitboards[6]
#define BLACK_KNIGHTS bitboards[7]
#define BLACK_BISHOPS bitboards[8]
#define BLACK_ROOKS bitboards[9]
#define BLACK_QUEENS bitboards[10]
#define BLACK_KING bitboards[11]

class Board {
  public:
	Board();
	void print();

	Bitboard getOccupied() { return occupancy[BOTH_OCCUPIED]; }
	Bitboard getOccupancy(int idx) { return occupancy[idx]; };
	int getSideToMove() const { return sideToMove; };
	int getEnPassantSquare() const { return enPassantSquare; }
	int getCastlingRights() const { return castlingRights; }

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

	Bitboard getOccupancy(int i) const { return occupancy[i]; }
	void updateOccupancy();
	bool makeMove(const Move &move);
	void unmakeMove();
	// ♟
	// ♞♛♙

  private:
	int pieceFinder(const int sq);

	Bitboard bitboards[12];
	Bitboard occupancy[3];

	// Square Colors
	const char *RESET = "\033[0m";
	const char *LIGHT_SQ = "\033[48;2;235;206;158m"; // Light wood
	const char *DARK_SQ = "\033[48;2;184;115;67m";	 // Dark wood

	// Piece Colors
	const char *BLACK_PIECE = "\033[38;2;0;0;0m";		// black text for white pieces
	const char *WHITE_PIECE = "\033[38;2;255;255;255m"; // white text for black pieces

	// Turn determinator, games start on white
	Move lastMove;
	int sideToMove = 0;
	int castlingRights = 15;
	int enPassantSquare = -1;
	int fiftyMoveCounter = 0;
	U64 zobristKey = 0ULL;
};
