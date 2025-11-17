#include "MoveGen.h"
#include <iostream>
// #include "chess-types.h"

MoveGen::MoveGen() {
  for (int sq = 0; sq < 64; sq++) {
	precomputePawns(sq);
	precomputeKnights(sq);
	precomputeKings(sq);
  }
}

void MoveGen::precomputePawns(int sq) {
  // Precompute Pawn attacks
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

void MoveGen::precomputeKnights(int sq) {
  // Precompute Knight movements/attacks
  if (sq <= 45 && sq >= 18) {
	KNIGHT_ATTACKS[sq].set_bit(sq - 17);
	KNIGHT_ATTACKS[sq].set_bit(sq + 15);
	KNIGHT_ATTACKS[sq].set_bit(sq - 15);
	KNIGHT_ATTACKS[sq].set_bit(sq + 17);
	KNIGHT_ATTACKS[sq].set_bit(sq + 6);
	KNIGHT_ATTACKS[sq].set_bit(sq - 10);
	KNIGHT_ATTACKS[sq].set_bit(sq + 10);
	KNIGHT_ATTACKS[sq].set_bit(sq - 6);
  } else {
	if ((sq & 7) != 0) {
	  if (sq > 16) 
		KNIGHT_ATTACKS[sq].set_bit(sq - 17);
	  if (sq < 48)
		KNIGHT_ATTACKS[sq].set_bit(sq + 15);
	  if ((sq & 7) != 1) {
		if (sq < 56)
		  KNIGHT_ATTACKS[sq].set_bit(sq + 6);
		if (sq > 8)
		  KNIGHT_ATTACKS[sq].set_bit(sq - 10);
	  }
	}
	if ((sq & 7) != 7) {
	  if (sq > 16) 
		KNIGHT_ATTACKS[sq].set_bit(sq - 15);
	  if (sq < 48)
		KNIGHT_ATTACKS[sq].set_bit(sq + 17);
	  if ((sq & 7) != 6) {
		if (sq < 56)
		  KNIGHT_ATTACKS[sq].set_bit(sq + 10);
		if (sq > 8)
		  KNIGHT_ATTACKS[sq].set_bit(sq - 6);
	  }
	}
  }
}

void MoveGen::precomputeKings(int sq) {
  // Precompute King movements/attacks
  if (sq > 7) 
	KING_MOVES[sq].set_bit(sq - 8);	// King down
  if ((sq & 7) != 0)
	KING_MOVES[sq].set_bit(sq - 1); // King left
  if ((sq & 7) != 7)
	KING_MOVES[sq].set_bit(sq + 1); // King right
  if (sq < 56)
	KING_MOVES[sq].set_bit(sq + 8); // King up
  if (sq > 7 && ((sq & 7) != 0))
	KING_MOVES[sq].set_bit(sq - 9); // King down left
  if (sq > 7 && ((sq & 7) != 7))
	KING_MOVES[sq].set_bit(sq - 7); // King down right
  if (sq < 56 && ((sq & 7) != 0))
	KING_MOVES[sq].set_bit(sq + 7); // King up left
  if (sq < 56 && ((sq & 7) != 7))
	KING_MOVES[sq].set_bit(sq + 9); // King up right
}

void MoveGen::generateAllMoves(Board &board, MoveList &moves) {
  moves.clear();

  // Loop over all pawns of side to move
  bool white = (board.getSideToMove() == ecWhite);

  generatePawnMoves(board, moves, white);
  generateKnightMoves(board, moves, white);
  generateKingMoves(board, moves, white);

}

void MoveGen::generatePawnMoves(Board &board, MoveList &moves, bool white) {
  Bitboard pawns = white ? board.getWhitePawns() : board.getBlackPawns();
  int from_sq;
  while ((from_sq = pawns.pop_lsb()) != -1) {
    Bitboard targets = getPawnMoves(from_sq, white);

    int to_sq;
    while ((to_sq = targets.pop_lsb()) != -1) {
      // Check if target is empty (quiet) or opponent (capture)
	  bool is_capture = board.getOccupancy(!white).get_bit(to_sq);
      bool promotion = white ? (to_sq / 8 == 7) : (to_sq / 8 == 0);
	  if (!is_capture && (abs(from_sq - to_sq) % 8 != 0)) { // Only add capturing pawn move if it is actually capturing
		continue;
	  }
      moves.push_back({from_sq, to_sq, promotion, is_capture, false, false});
    }
  }
}

void MoveGen::generateKnightMoves(Board &board, MoveList &moves, bool white) {
  Bitboard knights = white ? board.getWhiteKnights() : board.getBlackKnights();
  int from_sq;
  while ((from_sq = knights.pop_lsb()) != -1) {
    Bitboard targets = KNIGHT_ATTACKS[from_sq];
	int to_sq;

    while ((to_sq = targets.pop_lsb()) != -1) {
      // Check if target is empty (quiet) or opponent (capture)
	  bool is_capture = board.getOccupancy(!white).get_bit(to_sq);
	  if (board.getOccupancy(white ? 0 : 1).get_bit(to_sq)) { // Can't capture own piece
		continue;
	  }
      moves.push_back({from_sq, to_sq, false, is_capture, false, false});
    }
  }
}

void MoveGen::generateKingMoves(Board &board, MoveList &moves, bool white) {
  Bitboard king = white ? board.getWhiteKing() : board.getBlackKing();
  int from_sq;
  while ((from_sq = king.pop_lsb()) != -1) {
    Bitboard targets = KING_MOVES[from_sq];
	int to_sq;

    while ((to_sq = targets.pop_lsb()) != -1) {
      // Check if target is empty (quiet) or opponent (capture)
	  bool is_capture = board.getOccupancy(!white).get_bit(to_sq);
	  if (board.getOccupancy(white ? 0 : 1).get_bit(to_sq)) { // Can't capture own piece
		continue;
	  }
      moves.push_back({from_sq, to_sq, false, is_capture, false, false});
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

Bitboard MoveGen::getKnightMoves(const int from_square, bool white) {
  return Bitboard(0); 
}
