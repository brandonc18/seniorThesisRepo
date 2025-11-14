#include "Board.h"
#include "MoveGen.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main() {
  Board board;
  MoveList moves;
  // Move move(board.e2, board.e4, false, false, false, false);
  // Move mo(board.e2, board.e4);
  MoveGen g;
  g.generateAllMoves(board, moves);
  // system("clear");
  board.print();
  // board.makeMove(move);
    // system("clear");
    // board.print();

  for (int i = 0; i < moves.size(); i++) {
    cout << moves[i].from_square << " to " << moves[i].to_square << endl;
  }
  return 0;
}
