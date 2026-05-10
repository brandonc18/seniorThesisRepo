#include "Transposition.h"

TranspositionTable::TranspositionTable() {
	size = 1ULL << 20; // Set size of table to be > 1 million entries

	table.resize(size);
}

bool TranspositionTable::probe(U64 key, int depth, int ply, int alpha, int beta, int &score, Move &bestMove) {
	// Find index based on the key
	size_t idx = key % size;
	const TTEntry &entry = table[idx];

	// Check if the key matches what is at that index of the table
	// The depth should also be a least as deep as we want it to be
	if (entry.key == key && entry.depth >= depth) {
		bestMove = entry.bestMove; // Set best move to what is stored from previous searches

		if (entry.flag == TT_EXACT) {
			score = entry.score;
			return true;
		} else if (entry.flag == TT_ALPHA && entry.score <= alpha) {
			score = entry.score;
			return true;
		} else if (entry.flag == TT_BETA && entry.score >= beta) {
			score = entry.score;
			return true;
		}
	}
	return false;
}

void TranspositionTable::store(U64 key, int depth, int ply, int score, TTFlag flag, const Move &bestMove) {
	size_t idx = key % size;						  // Find index using the key
	table[idx] = {key, score, depth, flag, bestMove}; // Set TTEntry at that index to match given paramaters
}

void TranspositionTable::clear() { fill(table.begin(), table.end(), TTEntry{}); }
