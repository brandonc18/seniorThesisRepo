#include "MoveGen.h"

MoveGen::MoveGen() {
  for (int sq = 0; sq < 64; sq++) {
    // White attacks
    if ((sq & 7) != 0)
      PAWN_ATTACKS[sq][0].set_bit(sq + 7); // not a-file
    if ((sq & 7) != 7)
      PAWN_ATTACKS[sq][0].set_bit(sq + 9); // not h-file

    // Black attacks
    if ((sq & 7) != 0)
      PAWN_ATTACKS[sq][1].set_bit(sq - 9);
    if ((sq & 7) != 7)
      PAWN_ATTACKS[sq][1].set_bit(sq - 7);
  }
}

void MoveGen::generateAllMoves(Board &board, MoveList &moves) {
  moves.clear();

  // Loop over all pawns of side to move
  bool white = (board.getSideToMove() == Board::ecWhite);

  const Bitboard *pawns = white ? &board.getWhitePawns() : &board.getBlackPawns();

  int from_sq;
  while ((from_sq = pawns->pop_lsb()) != -1) {
    Bitboard targets = getPawnMoves(from_sq, white);

    int to_sq;
    while ((to_sq = targets.pop_lsb()) != -1) {
      // Check if target is empty (quiet) or opponent (capture)
      bool is_capture = board.getOccupied().get_bit(to_sq);
      bool promotion = white ? (to_sq / 8 == 7) : (to_sq / 8 == 0);

      moves.push_back({from_sq, to_sq, promotion, is_capture, false, false});
    }
  }
}

// pseudo-legal pawn moves from one square
Bitboard MoveGen::getPawnMoves(const int from_square, bool white) {
  // If not valid square return 0 possible moves
  if (from_square < 0 || from_square > 63) {
    return Bitboard(0);
  }

  int rank = from_square / 8; // Determine rank

  // Pawn shouldn't be able to be on promotion rank, but if so return 0 possible moves
  if ((white && rank == 7) || (!white && rank == 0)) {
    return Bitboard(0);
  }

  Bitboard moves = getPawnQuiets(from_square, white);
  moves = moves | getPawnCaptures(from_square, white); // Combine bitboards of attacks and silent pushes

  return moves; // Bitboard with all target squares set
}

// Helper to get forward moves
Bitboard MoveGen::getPawnQuiets(const int from_square, bool white) {
  Bitboard quiets;
  if (white) {
    if (from_square <= 55)
      quiets.set_bit(from_square + 8);
    if (from_square < 16)
      quiets.set_bit(from_square + 16);
  } else {
    if (from_square >= 16)
      quiets.set_bit(from_square - 8);
    if (from_square > 55)
      quiets.set_bit(from_square - 16);
  }

  return quiets;
}

// Helper to get attacks
Bitboard MoveGen::getPawnCaptures(const int from_square, bool white) {
  return PAWN_ATTACKS[from_square][white ? 0 : 1];
}
