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

	// Find what piece is moving (must exist)
	int movingPiece = pieceFinder(from);
	if (movingPiece == -1)
		return false;

	// If target square has an enemy piece capture it
	int capturedPiece = pieceFinder(to);
	if (capturedPiece != -1) {
		bool weAreWhite = (sideToMove == ecWhite);
		bool captureIsOpponent = (weAreWhite && capturedPiece >= 6) || // white captures black
								 (!weAreWhite && capturedPiece <= 5);  // black captures white

		if (!captureIsOpponent)
			return false; // trying to capture own piece is illegal

		// Remove the captured piece
		bitboards[capturedPiece].clear_bit(to);
	}

	// Move piece by clearing it at its previous location and adding it to its new one
	bitboards[movingPiece].clear_bit(from);
	bitboards[movingPiece].set_bit(to);

	// Switch side to move
	sideToMove = (sideToMove == ecWhite) ? ecBlack : ecWhite;

	// Update occupancy bitboards
	updateOccupancy();

	return true;
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
