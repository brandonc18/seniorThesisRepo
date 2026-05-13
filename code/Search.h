#pragma once

#include "Board.h"
#include "Evaluate.h"
#include "Move.h"
#include "MoveGen.h"
#include "Transposition.h"
#include <algorithm>
#include <cstdint>
#include <vector>

const int INF = 1000000;

class Search {
  public:
	Search(MoveGen &mg) : moveGen(mg), evaluator(), tt() {}
	Move findBestMove(Board &board, int depth);
	int alphaBeta(Board &board, int depth, int ply, int alpha, int beta);

  private:
	MoveGen &moveGen;
	Evaluate evaluator;
	TranspositionTable tt;

	int evaluate(Board &board) { return evaluator.evaluate(board); }
	int quiescenceSearch(Board &board, int alpha, int beta, int ply);
	int scoreMove(const Move &move, Board &board);
	void orderMoves(MoveList &moves, Board &board);
};
