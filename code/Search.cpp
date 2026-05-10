#include "Search.h"

Move Search::findBestMove(Board &board, int depth) {
	MoveList moves;
	moveGen.generateAllMoves(board, moves);
	moveGen.removeIllegalMoves(board, moves);

	if (moves.empty())
		return Move();

	// Order moves so that moves that are likely best are first to be searched
	orderMoves(moves, board);

	Move bestMove = moves[0];
	int bestValue = -INF; // Starting score that should always be updated

	for (const Move &move : moves) {
		Move m = move;
		if (board.makeMove(m)) {
			int value = -alphaBeta(board, depth - 1, 1, -INF, INF);
			board.unmakeMove(m);

			if (value > bestValue) {
				bestValue = value;
				bestMove = move;
			}
		}
	}

	return bestMove;
}

int Search::alphaBeta(Board &board, int depth, int ply, int alpha, int beta) {
	if (depth == 0) {
		return quiescenceSearch(board, alpha, beta, ply);
	}

	// Check if Position is in Transposition Table
	U64 hash = board.getZobristKey();
	Move ttBestMove;
	int ttScore;

	// Check the transpostion table to see if it has an entry at the correct depth
	if (tt.probe(hash, depth, ply, alpha, beta, ttScore, ttBestMove)) {
		return ttScore;
	}

	MoveList moves;
	moveGen.generateAllMoves(board, moves);
	moveGen.removeIllegalMoves(board, moves);

	if (moves.empty()) {
		// If there are no moves, check to see if the king is in check
		bool whiteToMove = (board.getSideToMove() == ecWhite);
		Bitboard kingBB = whiteToMove ? board.getWhiteKing() : board.getBlackKing();
		int kingSq = kingBB.pop_lsb();
		bool inCheck = (kingSq != -1) && moveGen.isSquareAttacked(board, kingSq, !whiteToMove);

		// If in check return score + distance from root, otherwise it's stalemate
		// The ply is to make mate in 1 better than a mate in 2
		return inCheck ? (-INF + ply) : 0;
	}

	orderMoves(moves, board);

	int bestScore = -INF; // Start with worst possible score
	TTFlag flag = TT_ALPHA;
	Move bestMove;

	for (const Move &move : moves) {
		Move m = move;
		if (board.makeMove(m)) {
			int score = -alphaBeta(board, depth - 1, ply + 1, -beta, -alpha);
			board.unmakeMove(m);

			if (score > bestScore) {
				bestScore = score; // This is the best score for this position
				bestMove = move;
			}
			if (score > alpha) { // Found a better score than we had before
				alpha = score;	 // Set this as the new alpha
				flag = TT_EXACT; // This is a usable move
			}
			if (alpha >= beta) { // Found a move that is better than what the opponent is willing to allow
				flag = TT_BETA;	 // Beta cutoff, scores that are greater than beta won't be allowed by opponent
				break;			 // Stop searching since we found something good, saves time searching
			}
		}
	}

	// Store in Transposition Table
	tt.store(hash, depth, ply, bestScore, flag, bestMove);

	return bestScore;
}

int Search::quiescenceSearch(Board &board, int alpha, int beta, int ply) {
	// TODO:
	// // Draw detection
	// if (board.isDraw()) {
	// 	return 0;
	// }

	// Check if Position is in Transposition Table
	U64 hash = board.getZobristKey();
	Move ttBestMove;
	int ttScore;

	// Check the transposition table to see if it has an entry for the current zobrist hash
	if (tt.probe(hash, 0, ply, alpha, beta, ttScore, ttBestMove)) {
		return ttScore;
	}

	int standPat = evaluate(board);
	bool whiteToMove = (board.getSideToMove() == ecWhite);
	int score = whiteToMove ? standPat : -standPat;

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;

	// Generate only captures
	MoveList moves;
	moveGen.generateAllMoves(board, moves);
	moveGen.removeIllegalMoves(board, moves);

	orderMoves(moves, board);

	for (const Move &move : moves) {
		if (!move.is_capture && !move.is_promotion) // only captures or promotions
			continue;

		Move m = move;
		if (board.makeMove(m)) {
			int qScore = -quiescenceSearch(board, -beta, -alpha, ply + 1);
			board.unmakeMove(m);

			if (qScore > alpha) {
				alpha = qScore;
			}
			if (alpha >= beta) {
				break; // beta cutoff
			}
		}
	}

	// Store in Transposition Table
	tt.store(hash, 0, ply, alpha, TT_EXACT, Move()); // depth 0 for QS

	return alpha;
}

int Search::scoreMove(const Move &move, Board &board) const {
	int score = 0;

	// En_Passent
	if (move.is_en_passant) {
		score += 2000;
	}

	// Castling
	if (move.is_castle) {
		score += 100000;
		return score;
	}

	// Promotions
	if (move.is_promotion > 0) {
		switch (move.is_promotion) {
		case 1:
			score += 10000; // Queen
			break;
		case 4:
			score += 6000; // Rook
			break;
		case 2:
			score += 4000; // Knight
			break;
		case 3:
			score += 4000; // Bishop
			break;
		}
	}

	// Captures
	if (move.is_capture) {
		int victimValue = 0;

		// Check which piece is on the target square
		if (board.getWhitePawns().get_bit(move.to_square) || board.getBlackPawns().get_bit(move.to_square)) {
			victimValue = 100;
		} else if (board.getWhiteKnights().get_bit(move.to_square) || board.getBlackKnights().get_bit(move.to_square)) {
			victimValue = 300;
		} else if (board.getWhiteBishops().get_bit(move.to_square) || board.getBlackBishops().get_bit(move.to_square)) {
			victimValue = 300;
		} else if (board.getWhiteRooks().get_bit(move.to_square) || board.getBlackRooks().get_bit(move.to_square)) {
			victimValue = 500;
		} else if (board.getWhiteQueens().get_bit(move.to_square) || board.getBlackQueens().get_bit(move.to_square)) {
			victimValue = 900;
		}

		score += victimValue;
	}

	return score;
}

void Search::orderMoves(MoveList &moves, Board &board) const {
	// Order moves from Highest Score to Lowest
	for (int i = 0; i < moves.size(); i++) {
		int bestIdx = i;
		for (int j = i + 1; j < moves.size(); j++) {
			if (scoreMove(moves[j], board) > scoreMove(moves[bestIdx], board)) {
				bestIdx = j;
			}
		}
		if (bestIdx != i) {
			swap(moves[i], moves[bestIdx]);
		}
	}
}
