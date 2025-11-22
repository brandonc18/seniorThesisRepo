#include "Board.h"
// #include "chess-types.h"
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

	// print();
	updateOccupancy();
}

// Backgrounds
constexpr const char *RESET = "\033[0m";
constexpr const char *LIGHT_SQ = "\033[48;2;235;206;158m"; // Light wood
constexpr const char *DARK_SQ = "\033[48;2;184;115;67m";   // Dark wood

// Foreground colors for the pieces
constexpr const char *BLACK_PIECE = "\033[38;2;0;0;0m";		  // black text for white pieces
constexpr const char *WHITE_PIECE = "\033[38;2;255;255;255m"; // white text for black pieces

void Board::print() {
	std::cout << "\n ";

	for (int row = 7; row >= 0; --row) {
		std::cout << " " << (row + 1) << " "; // rank number

		for (int col = 0; col < 8; ++col) {
			int sq = row * 8 + col;

			// Determine piece and its color
			char piece = ' ';
			const char *fg = nullptr; // foreground (text) color

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

			// Print: background + foreground
			std::cout << bg;
			if (piece != ' ')
				std::cout << fg; // only apply text color if there's a piece
			std::cout << " " << piece << " ";
			std::cout << RESET;
		}
		std::cout << "\n ";
	}

	std::cout << "    a  b  c  d  e  f  g  h \n\n";
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
	int from_sq = move.from_square;
	int to_sq = move.to_square;
	int piece = pieceFinder(from_sq);

	if (piece == -1) {
		return false;
	}
	if (move.is_capture) {
		int capturedPiece = pieceFinder(to_sq);
		if ((sideToMove == ecWhite && capturedPiece > 5) || (sideToMove == ecBlack && capturedPiece < 6)) {
			// Capture piece from opposing side and move piece to location of capture
			bitboards[capturedPiece].clear_bit(to_sq);
			bitboards[piece].clear_bit(from_sq);
			bitboards[piece].set_bit(to_sq);
		} else {
			return false;
		}
	} else if (!move.is_capture && pieceFinder(to_sq) == -1) {
		// Move piece quietly
		bitboards[piece].clear_bit(from_sq);
		bitboards[piece].set_bit(to_sq);
	}
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
