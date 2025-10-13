#pragma once
#include <cstdint>

using namespace std;
using U64 = uint64_t;

class Board {
public:
	Board();
	U64 occupied();
	void print();
private:
	// Bitboards for each piece type
	U64 white_pawns, white_knights, white_bishops, white_rooks, white_queens, white_king;
    U64 black_pawns, black_knights, black_bishops, black_rooks, black_queens, black_king;
	// Turn determinator, games start on white
	bool white_turn = true;
};
