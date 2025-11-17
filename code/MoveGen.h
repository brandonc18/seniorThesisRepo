#pragma once
#include "Board.h"
#include "Move.h"

using MoveList = vector<Move>;

class MoveGen {
public:
  MoveGen();
  void generateAllMoves(Board &board, MoveList &moves);
  // ... knight, sliding, etc.
private:
  void generatePawnMoves(Board &board, MoveList &moves, bool white);
  void generateKnightMoves(Board &board, MoveList &moves, bool white);
  void generateKingMoves(Board &board, MoveList &moves, bool white);
  void precomputePawns(int sq);
  void precomputeKnights(int sq);
  void precomputeKings(int sq);
  Bitboard getPawnQuiets(const int from_square, bool white);
  Bitboard getPawnCaptures(const int from_square, bool white);
  Bitboard getPawnMoves(const int from_square, bool white);
  Bitboard getKnightMoves(const int from_square, bool white);

  Bitboard PAWN_ATTACKS[64][2];
  Bitboard KNIGHT_ATTACKS[64]; 
  Bitboard KING_MOVES[64];
};
