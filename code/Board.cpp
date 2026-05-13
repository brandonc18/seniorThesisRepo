#include "Board.h"
#include "chess-types.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

array<array<U64, 64>, 12> Board::ZobristPieces{};
array<U64, 16> Board::ZobristCastling{};
array<U64, 8> Board::ZobristEnPassant{};
U64 Board::ZobristSide = 0ULL;

Board::Board() {
	// White pieces
	WHITE_PAWNS.set_raw(0x000000000000FF00ULL);	  // Rank 2 (squares 8-15)
	WHITE_KNIGHTS.set_raw(0x0000000000000042ULL); // b1 (1), g1 (6)
	WHITE_BISHOPS.set_raw(0x0000000000000024ULL); // c1 (2), f1 (5)
	WHITE_ROOKS.set_raw(0x0000000000000081ULL);	  // a1 (0), h1 (7)
	WHITE_QUEENS.set_raw(0x0000000000000008ULL);  // d1 (3)
	WHITE_KING.set_raw(0x0000000000000010ULL);	  // e1 (4)

	// Black pieces
	BLACK_PAWNS.set_raw(0x00FF000000000000ULL);	  // Rank 7 (squares 48-55)
	BLACK_KNIGHTS.set_raw(0x4200000000000000ULL); // b8 (57), g8 (62)
	BLACK_BISHOPS.set_raw(0x2400000000000000ULL); // c8 (58), f8 (61)
	BLACK_ROOKS.set_raw(0x8100000000000000ULL);	  // a8 (56), h8 (63)
	BLACK_QUEENS.set_raw(0x0800000000000000ULL);  // d8 (59)
	BLACK_KING.set_raw(0x1000000000000000ULL);	  // e8 (60)

	updateOccupancy();

	initZobrist();

	zobristKey = 0ULL;

	// Initialize full zobrist key from starting position
	for (int p = 0; p < 12; p++) {
		Bitboard bb = bitboards[p];
		int sq;
		while ((sq = bb.pop_lsb()) != -1) {
			zobristKey ^= ZobristPieces[p][sq];
		}
	}
	if (sideToMove == ecBlack)
		zobristKey ^= ZobristSide;
	if (castlingRights)
		zobristKey ^= ZobristCastling[castlingRights];
}

void Board::print() {
	cout << "\n ";

	for (int row = 7; row >= 0; row--) {
		cout << " " << (row + 1) << " "; // rank number

		for (int col = 0; col < 8; col++) {
			int sq = row * 8 + col;

			// Determine piece and its color
			char piece = ' ';
			const char *fg = nullptr; // text color

			if (WHITE_PAWNS.get_bit(sq)) {
				piece = 'P';
				fg = WHITE_PIECE;
			} else if (WHITE_KNIGHTS.get_bit(sq)) {
				piece = 'N';
				fg = WHITE_PIECE;
			} else if (WHITE_BISHOPS.get_bit(sq)) {
				piece = 'B';
				fg = WHITE_PIECE;
			} else if (WHITE_ROOKS.get_bit(sq)) {
				piece = 'R';
				fg = WHITE_PIECE;
			} else if (WHITE_QUEENS.get_bit(sq)) {
				piece = 'Q';
				fg = WHITE_PIECE;
			} else if (WHITE_KING.get_bit(sq)) {
				piece = 'K';
				fg = WHITE_PIECE;
			} else if (BLACK_PAWNS.get_bit(sq)) {
				piece = 'p';
				fg = BLACK_PIECE;
			} else if (BLACK_KNIGHTS.get_bit(sq)) {
				piece = 'n';
				fg = BLACK_PIECE;
			} else if (BLACK_BISHOPS.get_bit(sq)) {
				piece = 'b';
				fg = BLACK_PIECE;
			} else if (BLACK_ROOKS.get_bit(sq)) {
				piece = 'r';
				fg = BLACK_PIECE;
			} else if (BLACK_QUEENS.get_bit(sq)) {
				piece = 'q';
				fg = BLACK_PIECE;
			} else if (BLACK_KING.get_bit(sq)) {
				piece = 'k';
				fg = BLACK_PIECE;
			}

			// Background
			const char *bg = ((row + col) % 2) ? LIGHT_SQ : DARK_SQ;

			// Print with proper colors
			cout << bg;
			if (piece != ' ')
				cout << fg; // apply color to piece if piece is there
			cout << " " << piece << " ";
			cout << RESET;
		}
		cout << "\n ";
	}

	cout << "    a  b  c  d  e  f  g  h \n\n"; // File letter
}

void Board::updateOccupancy() {
	// Compute WHITE occupancy
	occupancy[WHITE_OCCUPIED] = WHITE_PAWNS | WHITE_KNIGHTS | WHITE_BISHOPS | WHITE_ROOKS | WHITE_QUEENS | WHITE_KING;
	// Compute BLACK occupancy
	occupancy[BLACK_OCCUPIED] = BLACK_PAWNS | BLACK_KNIGHTS | BLACK_BISHOPS | BLACK_ROOKS | BLACK_QUEENS | BLACK_KING;
	// Compute BOTH occupancy
	occupancy[BOTH_OCCUPIED] = occupancy[WHITE_OCCUPIED] | occupancy[BLACK_OCCUPIED];
}

bool Board::makeMove(Move &move) {
	const int from = move.from_square;
	const int to = move.to_square;

	// Find the piece that is moving
	int movingPiece = pieceFinder(from);
	if (movingPiece == -1)
		return false;

	bool isWhite = (sideToMove == ecWhite);

	move.movingPiece = movingPiece;
	move.movingIsWhite = isWhite;

	// Unmake move information
	move.oldCastlingRights = castlingRights;
	move.oldEnPassantSquare = enPassantSquare;
	move.oldZobristKey = zobristKey;
	move.capturedPiece = -1;

	if (!move.is_en_passant && !move.is_castle) {
		move.capturedPiece = pieceFinder(to);
	}

	int capturedPiece = move.capturedPiece;

	// // Update castling rights
	if (movingPiece == 5) {			// WHITE_KING
		castlingRights &= ~3;		// lose both white rights
	} else if (movingPiece == 11) { // BLACK_KING
		castlingRights &= ~12;		// lose both black rights
	} else if (movingPiece == 3) {	// WHITE_ROOKS
		if (from == a1)
			castlingRights &= ~1;
		if (from == h1)
			castlingRights &= ~2;
	} else if (movingPiece == 9) { // BLACK_ROOKS
		if (from == a8)
			castlingRights &= ~4;
		if (from == h8)
			castlingRights &= ~8;
	}

	// If we captured a rook on its original square, opponent loses that right
	if (capturedPiece == 3) { // white rook captured
		if (to == a1)
			castlingRights &= ~1;
		if (to == h1)
			castlingRights &= ~2;
	} else if (capturedPiece == 9) { // black rook captured
		if (to == a8)
			castlingRights &= ~4;
		if (to == h8)
			castlingRights &= ~8;
	}

	updateZobristKey(move);

	// int capturedPiece = move.capturedPiece;

	// Remove the moving piece from its original square
	bitboards[movingPiece].clear_bit(from);

	// Handle each type of move
	if (move.is_promotion) {
		// If promotion is through a capture
		if (capturedPiece != -1) {
			bitboards[capturedPiece].clear_bit(to);
		}
		// Promote to one of the following: (q, n, b, r)
		switch (move.is_promotion) {
		case 1:
			(isWhite ? WHITE_QUEENS : BLACK_QUEENS).set_bit(to);
			break;
		case 2:
			(isWhite ? WHITE_KNIGHTS : BLACK_KNIGHTS).set_bit(to);
			break;
		case 3:
			(isWhite ? WHITE_BISHOPS : BLACK_BISHOPS).set_bit(to);
			break;
		case 4:
			(isWhite ? WHITE_ROOKS : BLACK_ROOKS).set_bit(to);
			break;
		}

	} else if (move.is_castle) {
		bitboards[movingPiece].set_bit(to);

		// Determine kingside or queenside
		bool kingside = (to == g1 || to == g8);

		int rookFrom, rookTo;
		Bitboard *rookBB; // pointer to the correct rook bitboard

		if (isWhite) {
			rookBB = &WHITE_ROOKS;
			if (kingside) {
				rookFrom = h1;
				rookTo = f1;
			} else {
				rookFrom = a1;
				rookTo = d1;
			}
		} else {
			rookBB = &BLACK_ROOKS;
			if (kingside) {
				rookFrom = h8;
				rookTo = f8;
			} else {
				rookFrom = a8;
				rookTo = d8;
			}
		}

		// Move the rook
		rookBB->clear_bit(rookFrom);
		rookBB->set_bit(rookTo);
	} else if (move.is_en_passant) {
		// Pawn moves to the en passant square
		bitboards[movingPiece].set_bit(to);

		// Remove the captured pawn
		int epVictimSq = isWhite ? (to - 8) : (to + 8);
		(isWhite ? BLACK_PAWNS : WHITE_PAWNS).clear_bit(epVictimSq);
		move.capturedPiece = isWhite ? 6 : 0;
	} else {
		// Normal move or normal capture
		if (capturedPiece != -1) {
			bitboards[capturedPiece].clear_bit(to);
		}
		bitboards[movingPiece].set_bit(to);
	}

	// Update en passant target square
	enPassantSquare = -1;
	if ((movingPiece == 0 || movingPiece == 6) && // white or black pawn
		(from - to == 16 || to - from == 16)) {	  // double push
		enPassantSquare = (from + to) / 2;
	}

	// Switch side to move
	sideToMove = isWhite ? ecBlack : ecWhite;

	// Update occupancy bitboards
	updateOccupancy();

	if (move.is_capture || (movingPiece == 0 || movingPiece == 6)) {
		fiftyMoveCounter = 0;
		history.clear(); // reset on capture or pawn move
	} else {
		fiftyMoveCounter++;
	}

	history.push_back(zobristKey);

	return true;
}

void Board::unmakeMove(const Move &move) {
	if (move.from_square == -1) {
		return;
	}

	const int from = move.from_square;
	const int to = move.to_square;
	bool wasWhiteToMove = (sideToMove == ecBlack);

	int movingPiece = pieceFinder(to);
	if (movingPiece == -1) {
		return;
	}

	if (move.is_promotion) {
		int promotedPiece = -1;
		if (wasWhiteToMove) {
			if (move.is_promotion == 1)
				promotedPiece = 4; // Queen
			else if (move.is_promotion == 2)
				promotedPiece = 1; // Knight
			else if (move.is_promotion == 3)
				promotedPiece = 2; // Bishop
			else if (move.is_promotion == 4)
				promotedPiece = 3; // Rook
		} else {
			if (move.is_promotion == 1)
				promotedPiece = 10; // Queen
			else if (move.is_promotion == 2)
				promotedPiece = 7; // Knight
			else if (move.is_promotion == 3)
				promotedPiece = 8; // Bishop
			else if (move.is_promotion == 4)
				promotedPiece = 9; // Rook
		}

		bitboards[promotedPiece].clear_bit(to);

		// Restore the pawn that promoted
		if (wasWhiteToMove) {
			bitboards[0].set_bit(from); // White pawn
		} else {
			bitboards[6].set_bit(from); // Black pawn
		}

		// Restore captured piece if there was one
		if (move.capturedPiece != -1) {
			bitboards[move.capturedPiece].set_bit(to);
		}
	} else if (move.is_castle) {
		// King back to original square
		bitboards[movingPiece].clear_bit(to);
		bitboards[movingPiece].set_bit(from);

		// Rook back
		bool kingside = false;
		if (wasWhiteToMove) {
			kingside = (to == g1);
		} else {
			kingside = (to == g8);
		}

		int rookFrom, rookTo, rookIdx;

		if (wasWhiteToMove) {
			rookIdx = 3; // White rook
			if (kingside) {
				rookFrom = f1;
				rookTo = h1;
			} else {
				rookFrom = d1;
				rookTo = a1;
			}
		} else {
			rookIdx = 9; // Black rook
			if (kingside) {
				rookFrom = f8;
				rookTo = h8;
			} else {
				rookFrom = d8;
				rookTo = a8;
			}
		}

		bitboards[rookIdx].clear_bit(rookFrom);
		bitboards[rookIdx].set_bit(rookTo);
	} else if (move.is_en_passant) {
		// Move pawn back
		bitboards[movingPiece].clear_bit(to);
		bitboards[movingPiece].set_bit(from);

		// Restore captured pawn
		int epVictimSq = 0;
		if (wasWhiteToMove) {
			epVictimSq = to - 8;
		} else {
			epVictimSq = to + 8;
		}
		bitboards[wasWhiteToMove ? 6 : 0].set_bit(epVictimSq);

	} else {
		// Normal move or capture
		bitboards[movingPiece].clear_bit(to);
		bitboards[movingPiece].set_bit(from);

		// Restore captured piece if there was one
		if (move.capturedPiece != -1) {
			bitboards[move.capturedPiece].set_bit(to);
		}
	}

	castlingRights = move.oldCastlingRights;
	enPassantSquare = move.oldEnPassantSquare;
	zobristKey = move.oldZobristKey;
	sideToMove = wasWhiteToMove ? ecWhite : ecBlack;

	updateOccupancy();
	if (!history.empty()) {
		history.pop_back(); // Restore previous history from before the move
	}
}

bool Board::isDraw() {
	if (fiftyMoveCounter >= 100) {
		return true;
	}

	// Check for threefold repetition
	if (history.size() >= 6) {
		U64 current = history.back();
		int count = 1;
		for (int i = history.size() - 2; i >= 0; i--) {
			if (history[i] == current) {
				count++;
				if (count >= 3)
					return true; // It's in the history twice, plus the current state
			}
		}
	}
	return false;
}

int Board::pieceFinder(const int sq) {
	// Find the piece at that position, -1 if no piece there at all
	if (!occupancy[2].get_bit(sq))
		return -1;
	else if (WHITE_PAWNS.get_bit(sq))
		return 0;
	else if (WHITE_KNIGHTS.get_bit(sq))
		return 1;
	else if (WHITE_BISHOPS.get_bit(sq))
		return 2;
	else if (WHITE_ROOKS.get_bit(sq))
		return 3;
	else if (WHITE_QUEENS.get_bit(sq))
		return 4;
	else if (WHITE_KING.get_bit(sq))
		return 5;
	else if (BLACK_PAWNS.get_bit(sq))
		return 6;
	else if (BLACK_KNIGHTS.get_bit(sq))
		return 7;
	else if (BLACK_BISHOPS.get_bit(sq))
		return 8;
	else if (BLACK_ROOKS.get_bit(sq))
		return 9;
	else if (BLACK_QUEENS.get_bit(sq))
		return 10;
	else if (BLACK_KING.get_bit(sq))
		return 11;
	return -1;
}

void Board::initZobrist() {
	mt19937_64 rng(123456789ULL); // Consistent Seed

	for (int piece = 0; piece < 12; piece++) {
		for (int sq = 0; sq < 64; sq++) {
			// Fill the piece-square table with unique 64-bit numbers
			ZobristPieces[piece][sq] = rng();
		}
	}

	for (int i = 0; i < 16; i++) {
		// Creates unique numbers for each possible castling state
		ZobristCastling[i] = rng();
	}
	for (int i = 0; i < 8; i++) {
		// Creates unique numbers for each file for En passant
		ZobristEnPassant[i] = rng();
	}
	ZobristSide = rng(); // Creates random number for the side to move
}

void Board::updateZobristKey(const Move &move) {
	const int from = move.from_square;
	const int to = move.to_square;

	int movingPiece = move.movingPiece;
	bool isWhite = move.movingIsWhite;

	// Remove moving piece (king) from original square
	zobristKey ^= ZobristPieces[movingPiece][from];

	// Remove captured piece, if any
	if (move.capturedPiece != -1) {
		zobristKey ^= ZobristPieces[move.capturedPiece][to];
	}

	if (move.is_promotion) {
		// promotion numbers {null, queen, knight, bishop, rook}
		const int WhitePromotion[5] = {0, 4, 1, 2, 3};
		const int BlackPromotion[5] = {0, 10, 7, 8, 9};
		int newPiece = isWhite ? WhitePromotion[move.is_promotion] : BlackPromotion[move.is_promotion];
		zobristKey ^= ZobristPieces[newPiece][to];
	} else if (move.is_en_passant) {
		zobristKey ^= ZobristPieces[movingPiece][to];
		int epVictimSq = isWhite ? (to - 8) : (to + 8);
		zobristKey ^= ZobristPieces[isWhite ? 6 : 0][epVictimSq];
	} else {
		zobristKey ^= ZobristPieces[movingPiece][to];
	}

	// Castling
	if (move.is_castle) {
		bool kingside = (to == g1 || to == g8);
		int rookFrom, rookTo;
		int rookPieceIdx = isWhite ? 3 : 9; // WHITE_ROOKS or BLACK_ROOKS

		if (isWhite) {
			rookFrom = kingside ? h1 : a1;
			rookTo = kingside ? f1 : d1;
		} else {
			rookFrom = kingside ? h8 : a8;
			rookTo = kingside ? f8 : d8;
		}

		// Remove rook from original square
		zobristKey ^= ZobristPieces[rookPieceIdx][rookFrom];
		// Place rook on new square
		zobristKey ^= ZobristPieces[rookPieceIdx][rookTo];
	}

	// Side flip
	zobristKey ^= ZobristSide;

	// Castling rights
	if (castlingRights != move.oldCastlingRights) {
		if (move.oldCastlingRights != 0)
			zobristKey ^= ZobristCastling[move.oldCastlingRights];
		if (castlingRights != 0)
			zobristKey ^= ZobristCastling[castlingRights];
	}

	// En passant
	if (move.oldEnPassantSquare != -1)
		zobristKey ^= ZobristEnPassant[move.oldEnPassantSquare & 7];

	if ((movingPiece == 0 || movingPiece == 6) && abs(from - to) == 16) {
		int epSq = (from + to) / 2;
		zobristKey ^= ZobristEnPassant[epSq & 7];
	}
}
