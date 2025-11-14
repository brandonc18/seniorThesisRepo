#pragma once
#include "Board.h"
#include "Move.h"
#include <vector>

using MoveList = vector<Move>;

class MoveGen {
public:
  MoveGen();
  void generateAllMoves(Board &board, MoveList &moves);
  // ... knight, sliding, etc.
private:
  Bitboard getPawnQuiets(const int from_square, bool white);
  Bitboard getPawnCaptures(const int from_square, bool white);
  Bitboard getPawnMoves(const int from_square, bool white);
  Bitboard getKnightMoves(const int from_square, bool white);

  Bitboard PAWN_ATTACKS[64][2];
  Bitboard KNIGHT_ATTACKS[64][2]; 
};
