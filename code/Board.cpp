#include "Board.h"
#include <iostream>


Board::Board() {
	// White pieces
    WHITE_PAWNS.set_raw(0x000000000000FF00ULL);      // Rank 2 (squares 8-15)
    WHITE_KNIGHTS.set_raw(0x0000000000000042ULL);    // b1 (1), g1 (6)
    WHITE_BISHOPS.set_raw(0x0000000000000024ULL);    // c1 (2), f1 (5)
    WHITE_ROOKS.set_raw(0x0000000000000081ULL);      // a1 (0), h1 (7)
    WHITE_QUEENS.set_raw(0x0000000000000008ULL);     // d1 (3)
    WHITE_KING.set_raw(0x0000000000000010ULL);       // e1 (4)

    // Black pieces
    BLACK_PAWNS.set_raw(0x00FF000000000000ULL);      // Rank 7 (squares 48-55)
    BLACK_KNIGHTS.set_raw(0x4200000000000000ULL);    // b8 (57), g8 (62)
    BLACK_BISHOPS.set_raw(0x2400000000000000ULL);    // c8 (58), f8 (61)
    BLACK_ROOKS.set_raw(0x8100000000000000ULL);      // a8 (56), h8 (63)
    BLACK_QUEENS.set_raw(0x0800000000000000ULL);     // d8 (59)
    BLACK_KING.set_raw(0x1000000000000000ULL);       // e8 (60)
	
	print();
	updateOccupancy();
}

void Board::print() {
	// U64 occupied = this->occupied();
	for (int row = 7; row >= 0; row--) {
		cout << row + 1 << " ";
		for (int col = 0; col < 8; col++) {
			int sq = row * 8 + col;
			char p = '-';

            if (WHITE_PAWNS.get_bit(sq)) p = 'P';
            else if (WHITE_KNIGHTS.get_bit(sq)) p = 'N';
            else if (WHITE_BISHOPS.get_bit(sq)) p = 'B';
            else if (WHITE_ROOKS.get_bit(sq)) p = 'R';
            else if (WHITE_QUEENS.get_bit(sq)) p = 'Q';
            else if (WHITE_KING.get_bit(sq)) p = 'K';
            else if (BLACK_PAWNS.get_bit(sq)) p = 'p';
            else if (BLACK_KNIGHTS.get_bit(sq)) p = 'n';
            else if (BLACK_BISHOPS.get_bit(sq)) p = 'b';
            else if (BLACK_ROOKS.get_bit(sq)) p = 'r';
            else if (BLACK_QUEENS.get_bit(sq)) p = 'q';
            else if (BLACK_KING.get_bit(sq)) p = 'k';

			cout << p << ' ';
		}
			cout << endl;
	}
	cout << "  a b c d e f g h\n\n";
}

void Board::updateOccupancy() {
    // Compute WHITE occupancy
    occupancy[WHITE_OCCUPIED] = WHITE_PAWNS | WHITE_KNIGHTS | WHITE_BISHOPS | WHITE_ROOKS | WHITE_QUEENS | WHITE_KING;
    // Compute BLACK occupancy
    occupancy[BLACK_OCCUPIED] = BLACK_PAWNS | BLACK_KNIGHTS | BLACK_BISHOPS | BLACK_ROOKS | BLACK_QUEENS | BLACK_KING;
    // Compute BOTH occupancy
    occupancy[BOTH_OCCUPIED] = occupancy[WHITE_OCCUPIED] | occupancy[BLACK_OCCUPIED];
}

bool Board::makeMove(const Move& move) {
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
	if (!occupancy[2].get_bit(sq)) return -1;
	else if (WHITE_PAWNS.get_bit(sq)) return 0;
	else if (WHITE_KNIGHTS.get_bit(sq)) return 1;
	else if (WHITE_BISHOPS.get_bit(sq)) return 2;
	else if (WHITE_ROOKS.get_bit(sq)) return 3;
	else if (WHITE_QUEENS.get_bit(sq)) return 4;
	else if (WHITE_KING.get_bit(sq)) return 5;
	else if (BLACK_PAWNS.get_bit(sq)) return 6;
	else if (BLACK_KNIGHTS.get_bit(sq)) return 7;
	else if (BLACK_BISHOPS.get_bit(sq)) return 8;
	else if (BLACK_ROOKS.get_bit(sq)) return 9;
	else if (BLACK_QUEENS.get_bit(sq)) return 10;
	else if (BLACK_KING.get_bit(sq)) return 11;
	return -1;
}
