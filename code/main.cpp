#include <iostream>

using namespace std;
using U64 = uint64_t;
struct Board {
	// Bitboards for each piece type
	U64 white_pawns, white_knights, white_bishops, white_rooks, white_queens, white_king;
    U64 black_pawns, black_knights, black_bishops, black_rooks, black_queens, black_king;
	// Turn determinator, games start on white
	bool white_turn = true;
	// ...
	// More likely needed at some point
	//
	
	Board() {
		white_pawns = 0x000000000000FF00ULL;	// Rank 2
		white_knights = 0x0000000000000042ULL;	// B1 and G1
		white_bishops = 0x0000000000000024ULL;	// C1 and F1
		white_rooks = 0x0000000000000081ULL;	// A1 and H1
		white_queens = 0x0000000000000010ULL;	// D1
		white_king = 0x0000000000000008ULL;		// E1
		black_pawns = 0x00FF000000000000ULL;	// Rank 7
		black_knights = 0x4200000000000000ULL;	// B8 and G8
		black_bishops = 0x2400000000000000ULL;	// C8 and F8
		black_rooks = 0x8100000000000000ULL;	// A8 and H8
		black_queens = 0x1000000000000000ULL;	// D8
		black_king = 0x0800000000000000ULL;		// E8
	}
}

int main()
{
    cout<< "Chess Engine Project" <<endl;

    return 0;
}
