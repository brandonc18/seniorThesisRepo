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

Bitboard Board::occupied() {
    return occupancy[BOTH_OCCUPIED]; // Return precomputed occupancy for all pieces
}

void Board::updateOccupancy() {
    // Compute WHITE occupancy
    occupancy[WHITE_OCCUPIED] = WHITE_PAWNS | WHITE_KNIGHTS | WHITE_BISHOPS | WHITE_ROOKS | WHITE_QUEENS | WHITE_KING;
    // Compute BLACK occupancy
    occupancy[BLACK_OCCUPIED] = BLACK_PAWNS | BLACK_KNIGHTS | BLACK_BISHOPS | BLACK_ROOKS | BLACK_QUEENS | BLACK_KING;
    // Compute BOTH occupancy
    occupancy[BOTH_OCCUPIED] = occupancy[WHITE_OCCUPIED] | occupancy[BLACK_OCCUPIED];
}
