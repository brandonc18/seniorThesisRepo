#pragma once
#include "Board.h"
#include "Move.h"

using MoveList = vector<Move>;

class MoveGen {
  public:
	MoveGen();
	void generateAllMoves(Board &board, MoveList &moves);
	void removeIllegalMoves(Board &board, MoveList &moves);
	bool isSquareAttacked(Board &board, int sq, bool attackerIsWhite);

	uint64_t perft(Board board, int depth);

  private:
	// Generate moves helpers for each piece to be used by generateAllMoves
	void generatePawnMoves(Board &board, MoveList &moves, bool white);
	void generateKnightMoves(Board &board, MoveList &moves, bool white);
	void generateKingMoves(Board &board, MoveList &moves, bool white);
	void generateRookMoves(Board &board, MoveList &moves, bool white);
	void generateBishopMoves(Board &board, MoveList &moves, bool white);
	void generateQueenMoves(Board &board, MoveList &moves, bool white);
	void generateCastling(Board &board, MoveList &moves, bool white);

	// Precompute moves for quicker move calculation
	void precomputePawns(int sq);
	void precomputeKnights(int sq);
	void precomputeKings(int sq);
	void precomputeRooks(int sq);
	void precomputeBishops(int sq);

	// Movement helpers for the generate functions
	Bitboard getPawnMoves(Board &board, const int from_square, bool white);
	Bitboard getRookAttacks(const int sq, U64 occupancy);
	Bitboard getBishopAttacks(const int sq, U64 occupancy);
	Bitboard getQueenAttacks(const int sq, U64 occupancy) { return getRookAttacks(sq, occupancy) | getBishopAttacks(sq, occupancy); }

	Bitboard PAWN_ATTACKS[64][2];
	Bitboard KNIGHT_ATTACKS[64];
	Bitboard KING_MOVES[64];
	Bitboard ROOK_ATTACKS[64][4096];
	Bitboard BISHOP_ATTACKS[64][512];
};
