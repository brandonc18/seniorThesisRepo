#include "Board.h"
#include "MoveGen.h"
#include <cstdlib>


int main() {
	Board board;
	MoveList moves;
	Move move(board.e2, board.e4, false, false, false, false);
	Move mo(board.e2, board.e4);
	MoveGen::generateAllMoves(board, moves);
	system("clear");
	board.print();
	board.makeMove(move);
	system("clear");
	board.print();

    return 0;
}
