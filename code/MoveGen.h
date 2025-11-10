#pragma once
#include "Board.h"
#include "Move.h"
#include <vector>

using MoveList = vector<Move>;

class MoveGen {
public:
  static void init();
  const static void generateAllMoves(const Board &board, MoveList &moves);
  // ... knight, sliding, etc.
private:
  static Bitboard getPawnQuiets(const int from_square, bool white);
  static Bitboard getPawnCaptures(const int from_square, bool white);
  static Bitboard getPawnMoves(const int from_square, bool white);
  // Precomputed attack tables (e.g., pawn_attacks[64][2])
  static Bitboard PAWN_ATTACKS[64][2]; // White/black
  static bool initialized;
};
