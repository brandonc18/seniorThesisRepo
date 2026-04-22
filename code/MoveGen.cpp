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
	KNIGHT_ATTACKS[sq].set_raw(0ULL);

	const int deltas[8] = {-17, -15, 15, 17, -10, -6, 6, 10};

	int fromFile = sq % 8;

	for (int delta : deltas) {
		int to = sq + delta;
		if (to >= 0 && to < 64) {
			int toFile = to % 8;
			if (abs(toFile - fromFile) <= 2) { // knights never change file by more than 2, prevents wrapping
				KNIGHT_ATTACKS[sq].set_bit(to);
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
	generateCastling(board, moves, white);
}

void MoveGen::removeIllegalMoves(Board &board, MoveList &moves) {
	MoveList legalMoves;

	// Remember who is about to move before we try any move
	bool whiteToMove = (board.getSideToMove() == ecWhite);

	for (const Move &pseudoMove : moves) {
		Board testBoard = board; // fresh copy for each move

		if (!testBoard.makeMove(pseudoMove)) {
			continue; // illegal special move
		}

		// After makeMove(), the side that just moved is the opposite of the original
		bool movedSideIsWhite = whiteToMove; // the side that made the move

		Bitboard kingBB = movedSideIsWhite ? testBoard.getWhiteKing() : testBoard.getBlackKing();

		int kingSq = kingBB.pop_lsb();
		if (kingSq == -1) {
			continue;
		}

		bool inCheck = isSquareAttacked(testBoard, kingSq, !movedSideIsWhite);

		if (!inCheck) {
			legalMoves.push_back(pseudoMove);
		}
	}

	moves = std::move(legalMoves);
}

bool MoveGen::isSquareAttacked(Board &board, int sq, bool attackerIsWhite) {
	// Do Pawns attack that sq?
	Bitboard pawnAttackBB = PAWN_ATTACKS[sq][attackerIsWhite ? 1 : 0];
	Bitboard pawnPieces = attackerIsWhite ? board.getWhitePawns() : board.getBlackPawns();

	if ((pawnAttackBB.raw() & pawnPieces.raw()) != 0)
		return true;

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
			bool capture = board.getOccupancy(!white).get_bit(to_sq);
			bool is_ep = (to_sq == board.getEnPassantSquare());

			// Is this a promotion move?
			bool isPromotion = white ? (to_sq >= 56) : (to_sq <= 7);

			if (isPromotion) {
				// Generate 4 moves: queen, rook, bishop, knight
				for (int p = 1; p <= 4; p++) {
					moves.push_back(Move(from_sq, to_sq, p, capture, false, is_ep));
				}
			} else {
				moves.push_back(Move(from_sq, to_sq, 0, capture, false, is_ep));
			}
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
			moves.push_back({from_sq, to_sq, 0, is_capture, false, false});
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
			moves.push_back({from_sq, to_sq, 0, is_capture, false, false});
		}
	}
}

void MoveGen::generateCastling(Board &board, MoveList &moves, bool white) {
	// King must still be on starting square
	int kingSq = white ? e1 : e8;
	if (!(white ? board.getWhiteKing() : board.getBlackKing()).get_bit(kingSq)) {
		return;
	}

	int rights = board.getCastlingRights();
	Bitboard occupied = board.getOccupied();

	if (white) {
		// White Kingside (O-O)
		if ((rights & 2) && !occupied.get_bit(f1) && !occupied.get_bit(g1) && !isSquareAttacked(board, e1, false) && // not in check
			!isSquareAttacked(board, f1, false) &&																	 // doesn't pass through check
			!isSquareAttacked(board, g1, false)) {																	 // doesn't land in check
			moves.push_back(Move(e1, g1, 0, false, true, false));
		}

		// White Queenside (O-O-O)
		if ((rights & 1) && !occupied.get_bit(b1) && !occupied.get_bit(c1) && !occupied.get_bit(d1) && !isSquareAttacked(board, e1, false) &&
			!isSquareAttacked(board, d1, false) && // passes through d1
			!isSquareAttacked(board, c1, false)) { // lands on c1
			moves.push_back(Move(e1, c1, 0, false, true, false));
		}
	} else {
		// Black Kingside (O-O)
		if ((rights & 8) && !occupied.get_bit(f8) && !occupied.get_bit(g8) && !isSquareAttacked(board, e8, true) && // not in check
			!isSquareAttacked(board, f8, true) &&																	// doesn't pass through check
			!isSquareAttacked(board, g8, true)) {																	// doesn't land in check
			moves.push_back(Move(e8, g8, 0, false, true, false));
		}

		// Black Queenside (O-O-O)
		if ((rights & 4) && !occupied.get_bit(b8) && !occupied.get_bit(c8) && !occupied.get_bit(d8) && !isSquareAttacked(board, e8, true) &&
			!isSquareAttacked(board, d8, true) && // passes through d8
			!isSquareAttacked(board, c8, true)) { // lands on c8
			moves.push_back(Move(e8, c8, 0, false, true, false));
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
			moves.push_back({sq, to, 0, capture, false, false});
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
			moves.push_back({sq, to, 0, capture, false, false});
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
			moves.push_back({sq, to, 0, capture, false, false});
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

uint64_t MoveGen::perft(Board board, int depth) {
	if (depth == 0) {
		return 1ULL;
	}

	MoveList moves;
	generateAllMoves(board, moves);
	removeIllegalMoves(board, moves);

	uint64_t leafCount = 0ULL;

	for (const Move &move : moves) {
		Board childBoard = board;
		if (childBoard.makeMove(move)) {
			leafCount += perft(childBoard, depth - 1);
		}
	}

	return leafCount;
}
