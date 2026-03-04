#include "MoveGen.h"
#include "Board.h"
#include "Magic.h"
#include <iostream>

MoveGen::MoveGen() {
	// Precompute moves for each square of the board
	for (int sq = 0; sq < 64; sq++) {
		precomputePawns(sq);
		precomputeKnights(sq);
		precomputeKings(sq);
		precomputeRooks(sq);
		precomputeBishops(sq);
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
		KING_MOVES[sq].set_bit(sq - 8); // King down
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

void MoveGen::precomputeRooks(int sq) {
	// Precompute Rook movements/attacks using magic bitboards
	U64 mask = rmask(sq);
	int bits = RBits[sq];
	int combinations = 1 << bits;

	for (int i = 0; i < combinations; i++) {
		U64 occupancy = index_to_uint64(i, bits, mask);
		U64 attacks = ratt(sq, occupancy);
		int index = transform(occupancy, RMagic[sq], bits);
		ROOK_ATTACKS[sq][index].set_raw(attacks);
	}
}

void MoveGen::precomputeBishops(int sq) {
	// Precompute Bishop movements/attacks using magic bitboards
	U64 mask = bmask(sq);
	int bits = BBits[sq];
	int combinations = 1 << bits;

	for (int i = 0; i < combinations; i++) {
		U64 occupancy = index_to_uint64(i, bits, mask);
		U64 attacks = batt(sq, occupancy);
		int index = transform(occupancy, BMagic[sq], bits);
		BISHOP_ATTACKS[sq][index].set_raw(attacks);
	}
}

void MoveGen::generateAllMoves(Board &board, MoveList &moves) {
	moves.clear();

	// Loop over all pawns of side to move
	bool white = (board.getSideToMove() == ecWhite);

	// Generate moves for each piece type
	generatePawnMoves(board, moves, white);
	generateKnightMoves(board, moves, white);
	generateBishopMoves(board, moves, white);
	generateRookMoves(board, moves, white);
	generateQueenMoves(board, moves, white);
	generateKingMoves(board, moves, white);
}

void MoveGen::removeIllegalMoves(Board &board, MoveList &moves) {
	MoveList legalMoves;
	bool sideToMove = (board.getSideToMove() == ecWhite);

	for (const Move &pseudoMove : moves) {
		// FIX: fresh copy for EVERY move (unmakeMove is empty)
		Board testBoard = board;
		testBoard.makeMove(pseudoMove);

		// Get the king of the side that just moved
		Bitboard kingBB = sideToMove ? testBoard.getWhiteKing() : testBoard.getBlackKing();
		int kingSq = kingBB.pop_lsb();

		if (kingSq != -1 && !isSquareAttacked(testBoard, kingSq, !sideToMove)) {
			legalMoves.push_back(pseudoMove); // If nothing is attacking there, it's a legal move
		}
	}

	moves = move(legalMoves); // Moves are now legal
}

bool MoveGen::isSquareAttacked(Board &board, int sq, bool attackerIsWhite) {
	// Do Pawns attack that sq?
	if (attackerIsWhite) {
		if ((PAWN_ATTACKS[sq][ecWhite].raw() & board.getWhitePawns().raw()) != 0)
			return true;
	} else {
		if ((PAWN_ATTACKS[sq][ecBlack].raw() & board.getBlackPawns().raw()) != 0)
			return true;
	}

	// Do the Knights?
	if ((KNIGHT_ATTACKS[sq].raw() & (attackerIsWhite ? board.getWhiteKnights().raw() : board.getBlackKnights().raw())) != 0)
		return true;

	// Does the opponent King?
	if ((KING_MOVES[sq].raw() & (attackerIsWhite ? board.getWhiteKing().raw() : board.getBlackKing().raw())) != 0)
		return true;

	// Do any of the sliding pieces?
	U64 occ = board.getOccupied().raw();

	Bitboard rookQueens = attackerIsWhite ? (board.getWhiteRooks() | board.getWhiteQueens()) : (board.getBlackRooks() | board.getBlackQueens());

	Bitboard bishopQueens = attackerIsWhite ? (board.getWhiteBishops() | board.getWhiteQueens()) : (board.getBlackBishops() | board.getBlackQueens());

	if ((getRookAttacks(sq, occ).raw() & rookQueens.raw()) != 0)
		return true;
	if ((getBishopAttacks(sq, occ).raw() & bishopQueens.raw()) != 0)
		return true;

	return false;
}

void MoveGen::generatePawnMoves(Board &board, MoveList &moves, bool white) {
	Bitboard pawns = white ? board.getWhitePawns() : board.getBlackPawns();

	int from_sq;
	while ((from_sq = pawns.pop_lsb()) != -1) {
		Bitboard targets = getPawnMoves(board, from_sq, white);

		int to_sq;
		while ((to_sq = targets.pop_lsb()) != -1) {
			// TODO: en passent not yet implemented
			bool capture = board.getOccupancy(!white).get_bit(to_sq);

			// TODO: promotions not yet implemented
			moves.push_back(Move(from_sq, to_sq, false, capture, false, to_sq == board.getEnPassantSquare()));
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
			// Check if target is quiet or capture
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
			// Check if target is quiet or capture
			bool is_capture = board.getOccupancy(!white).get_bit(to_sq);
			if (board.getOccupancy(white ? 0 : 1).get_bit(to_sq)) { // Can't capture own piece
				continue;
			}
			moves.push_back({from_sq, to_sq, false, is_capture, false, false});
		}
	}
}

Bitboard MoveGen::getPawnMoves(Board &board, int from_sq, bool white) {
	Bitboard targets;
	Bitboard occupied = board.getOccupied();
	Bitboard opponents = board.getOccupancy(white ? BLACK_OCCUPIED : WHITE_OCCUPIED);
	Bitboard empty = ~occupied;

	int direction = white ? 8 : -8;
	int to_single = from_sq + direction;

	// Single push
	if (to_single >= 0 && to_single < 64 && empty.get_bit(to_single)) {
		targets.set_bit(to_single);

		// Double push from starting rank
		if ((white && from_sq < 16) || (!white && from_sq >= 48)) {
			int to_double = to_single + direction;
			if (empty.get_bit(to_double)) {
				targets.set_bit(to_double);
			}
		}
	}

	// Captures (only to opponent pieces)
	Bitboard attacks = PAWN_ATTACKS[from_sq][white ? 0 : 1];
	targets |= (attacks & opponents);

	// En passant
	int ep = board.getEnPassantSquare();
	if (ep != -1 && attacks.get_bit(ep)) {
		targets.set_bit(ep);
	}

	return targets;
}

void MoveGen::generateQueenMoves(Board &board, MoveList &moves, bool white) {
	Bitboard pieces = white ? board.getWhiteQueens() : board.getBlackQueens();
	Bitboard own = board.getOccupancy(white ? WHITE_OCCUPIED : BLACK_OCCUPIED);

	int sq;
	while ((sq = pieces.pop_lsb()) != -1) {
		// Use both the rook and bishop magic bitboards to get queen moves
		// for the straight and vertical movements
		Bitboard attacks = getRookAttacks(sq, board.getOccupied().raw()) | getBishopAttacks(sq, board.getOccupied().raw());
		attacks &= ~own;

		int to;
		while ((to = attacks.pop_lsb()) != -1) {
			bool capture = board.getOccupancy(!white).get_bit(to);
			moves.push_back({sq, to, false, capture, false, false});
		}
	}
}

void MoveGen::generateRookMoves(Board &board, MoveList &moves, bool white) {
	Bitboard pieces = white ? board.getWhiteRooks() : board.getBlackRooks();
	Bitboard own = board.getOccupancy(white ? WHITE_OCCUPIED : BLACK_OCCUPIED);
	int sq;
	while ((sq = pieces.pop_lsb()) != -1) {
		Bitboard attacks = getRookAttacks(sq, board.getOccupied().raw());
		attacks &= ~own; // remove own pieces
		int to;
		while ((to = attacks.pop_lsb()) != -1) {
			bool capture = board.getOccupancy(!white).get_bit(to);
			moves.push_back({sq, to, false, capture, false, false});
		}
	}
}

void MoveGen::generateBishopMoves(Board &board, MoveList &moves, bool white) {
	Bitboard pieces = white ? board.getWhiteBishops() : board.getBlackBishops();
	Bitboard own = board.getOccupancy(white ? WHITE_OCCUPIED : BLACK_OCCUPIED);

	int sq;
	while ((sq = pieces.pop_lsb()) != -1) {
		Bitboard attacks = getBishopAttacks(sq, board.getOccupied().raw());
		attacks &= ~own; // remove own pieces (legal moves only)

		int to;
		while ((to = attacks.pop_lsb()) != -1) {
			bool capture = board.getOccupancy(!white).get_bit(to);
			moves.push_back({sq, to, false, capture, false, false});
		}
	}
}

Bitboard MoveGen::getRookAttacks(int sq, U64 occupancy) {
	occupancy &= rmask(sq);
	int index = transform(occupancy, RMagic[sq], RBits[sq]);
	return ROOK_ATTACKS[sq][index];
}

Bitboard MoveGen::getBishopAttacks(int sq, U64 occupancy) {
	occupancy &= bmask(sq);
	int index = transform(occupancy, BMagic[sq], BBits[sq]);
	return BISHOP_ATTACKS[sq][index];
}
