#pragma once
#include "Board.h"
#include "Move.h"
#include <vector>

enum TTFlag { TT_EXACT, TT_ALPHA, TT_BETA };

struct TTEntry {
	U64 key = 0; // Zobrist hash
	int score = 0;
	int depth = 0;
	TTFlag flag = TT_ALPHA;
	Move bestMove;
};

class TranspositionTable {
  public:
	TranspositionTable();

	bool probe(U64 key, int depth, int ply, int alpha, int beta, int &score, Move &bestMove);
	void store(U64 key, int depth, int ply, int score, TTFlag flag, const Move &bestMove);
	void clear();

  private:
	vector<TTEntry> table;
	size_t size;
};
