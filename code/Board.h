#pragma once

#include <cstdint>
#include "Bitboard.h"
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

	Bitboard getOccupied() const;
	Bitboard getOccupancy(int idx) { return occupancy[idx]; };
	int getSideToMove() const { return sideToMove; };
	int  getEnPassantSquare() const { return enPassantSquare; }

    const Bitboard& getWhitePawns() const { return WHITE_PAWNS; }
    const Bitboard& getBlackPawns() const { return BLACK_PAWNS; }

    const Bitboard& getOccupied() { return occupancy[BOTH_OCCUPIED]; }
    const Bitboard& getOccupancy(int i) const { return occupancy[i]; }
	void updateOccupancy();
	bool makeMove(const Move& move);
	// void unmakeMove(const Move& move);

	enum enumColor {
		ecWhite = 0,
		ecBlack = 1
	};

  	const int WHITE_OCCUPIED = 0;
	const int BLACK_OCCUPIED = 1;
	const int BOTH_OCCUPIED = 2;

	enum enumSquare {
		a1, b1, c1, d1, e1, f1, g1, h1,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a8, b8, c8, d8, e8, f8, g8, h8
	};

private:


	int pieceFinder(const int sq);

	Bitboard bitboards[12];
	Bitboard occupancy[3];

	// File and rank masks
	const U64 A_FILE        = 0x00000000000000FFULL;
	const U64 H_FILE        = 0xFF00000000000000ULL;
	const U64 FIRST_RANK    = 0x0101010101010101ULL;
	const U64 EIGHTH_RANK   = 0x8080808080808080ULL;

	// Diagonal masks
	const U64 A1_H8_DIAG    = 0x8040201008040201ULL;
	const U64 H1_A8_ANTIDIAG= 0x0102040810204080ULL;

	// Square color masks
	const U64 LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
	const U64 DARK_SQUARES  = 0xAA55AA55AA55AA55ULL;

	// Turn determinator, games start on white
	int sideToMove = 0;
	int castlingRights = 15;
	int enPassantSquare = -1;
	int fiftyMoveCounter = 0;
	U64 zobristKey = 0ULL;	
};
