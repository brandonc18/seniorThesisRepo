#include "Board.h"
#include <iostream>

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
}

void Board::print() {
	cout << "\n ";

	for (int row = 7; row >= 0; --row) {
		cout << " " << (row + 1) << " "; // rank number

		for (int col = 0; col < 8; ++col) {
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
				cout << fg; // apply color if piece is there
			cout << " " << piece << " ";
			cout << RESET;
		}
		cout << "\n ";
	}

	std::cout << "    a  b  c  d  e  f  g  h \n\n"; // File letter
}

void Board::updateOccupancy() {
	// Compute WHITE occupancy
	occupancy[WHITE_OCCUPIED] = WHITE_PAWNS | WHITE_KNIGHTS | WHITE_BISHOPS | WHITE_ROOKS | WHITE_QUEENS | WHITE_KING;
	// Compute BLACK occupancy
	occupancy[BLACK_OCCUPIED] = BLACK_PAWNS | BLACK_KNIGHTS | BLACK_BISHOPS | BLACK_ROOKS | BLACK_QUEENS | BLACK_KING;
	// Compute BOTH occupancy
	occupancy[BOTH_OCCUPIED] = occupancy[WHITE_OCCUPIED] | occupancy[BLACK_OCCUPIED];
}

bool Board::makeMove(const Move &move) {
	const int from = move.from_square;
	const int to = move.to_square;

	// Find the piece that is moving
	int movingPiece = pieceFinder(from);
	if (movingPiece == -1)
		return false;

	bool isWhite = (sideToMove == ecWhite);

	int capturedPiece = -1;
	if (!move.is_en_passant && !move.is_castle) {
		capturedPiece = pieceFinder(to);
	}

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

	// Update castling rights
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

	// Switch side to move
	sideToMove = isWhite ? ecBlack : ecWhite;

	// Update occupancy bitboards
	updateOccupancy();

	return true;
}

void Board::unmakeMove() {} // TODO: make unmake move function

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
