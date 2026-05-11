#include "Evaluate.h"

int Evaluate::materialScore(Bitboard pawns, Bitboard knights, Bitboard bishops, Bitboard rooks, Bitboard queens) {
	return pawns.count() * PIECE_SCORES_MATERIAL[0] + knights.count() * PIECE_SCORES_MATERIAL[1] + bishops.count() * PIECE_SCORES_MATERIAL[2] +
		   rooks.count() * PIECE_SCORES_MATERIAL[3] + queens.count() * PIECE_SCORES_MATERIAL[4];
}

int Evaluate::evaluate(Board &board) {
	//  Material Evaluation (White - Black)
	int material = materialScore(board.getWhitePawns(), board.getWhiteKnights(), board.getWhiteBishops(), board.getWhiteRooks(), board.getWhiteQueens()) -
				   materialScore(board.getBlackPawns(), board.getBlackKnights(), board.getBlackBishops(), board.getBlackRooks(), board.getBlackQueens());

	// Position Evaluation
	int mg = 0; // middlegame positional score
	int eg = 0; // endgame positional score

	// White pieces (positive score)
	addPieceSquareScores(board.getWhitePawns(), PAWN_SCORES_MG_POSITIONAL, PAWN_SCORES_EG_POSITIONAL, true, mg, eg);
	addPieceSquareScores(board.getWhiteKnights(), KNIGHT_SCORES_MG_POSITIONAL, KNIGHT_SCORES_EG_POSITIONAL, true, mg, eg);
	addPieceSquareScores(board.getWhiteBishops(), BISHOP_SCORES_MG_POSITIONAL, BISHOP_SCORES_EG_POSITIONAL, true, mg, eg);
	addPieceSquareScores(board.getWhiteRooks(), ROOK_SCORES_MG_POSITIONAL, ROOK_SCORES_EG_POSITIONAL, true, mg, eg);
	addPieceSquareScores(board.getWhiteQueens(), QUEEN_SCORES_MG_POSITIONAL, QUEEN_SCORES_EG_POSITIONAL, true, mg, eg);
	addPieceSquareScores(board.getWhiteKing(), KING_SCORES_MG_POSITIONAL, KING_SCORES_EG_POSITIONAL, true, mg, eg);

	// Black pieces (negative score)
	addPieceSquareScores(board.getBlackPawns(), PAWN_SCORES_MG_POSITIONAL, PAWN_SCORES_EG_POSITIONAL, false, mg, eg);
	addPieceSquareScores(board.getBlackKnights(), KNIGHT_SCORES_MG_POSITIONAL, KNIGHT_SCORES_EG_POSITIONAL, false, mg, eg);
	addPieceSquareScores(board.getBlackBishops(), BISHOP_SCORES_MG_POSITIONAL, BISHOP_SCORES_EG_POSITIONAL, false, mg, eg);
	addPieceSquareScores(board.getBlackRooks(), ROOK_SCORES_MG_POSITIONAL, ROOK_SCORES_EG_POSITIONAL, false, mg, eg);
	addPieceSquareScores(board.getBlackQueens(), QUEEN_SCORES_MG_POSITIONAL, QUEEN_SCORES_EG_POSITIONAL, false, mg, eg);
	addPieceSquareScores(board.getBlackKing(), KING_SCORES_MG_POSITIONAL, KING_SCORES_EG_POSITIONAL, false, mg, eg);

	// Game Phase (0 is endgame while 24 is middlegame)
	int phase = 0;
	phase += (board.getWhiteKnights().count() + board.getBlackKnights().count()) * 1;
	phase += (board.getWhiteBishops().count() + board.getBlackBishops().count()) * 1;
	phase += (board.getWhiteRooks().count() + board.getBlackRooks().count()) * 2;
	phase += (board.getWhiteQueens().count() + board.getBlackQueens().count()) * 4;
	phase = min(phase, 24);

	// Linear interpolation from MG to EG
	int positional = (mg * phase + eg * (24 - phase)) / 24;

	// Reward certain moves
	positional += rewardCastling(board);
	positional += rewardPawnShield(board);
	positional += rewardPawnAdvancement(board);
	positional += rewardKnightDevelopment(board);

	if (phase <= 12) {
		positional += rewardEndgameProgress(board);
	}

	// Final score
	return material + positional;
}

int Evaluate::rewardCastling(Board &board) const {
	int score = 0;

	if (board.getWhiteKing().get_bit(g1) || board.getWhiteKing().get_bit(c1)) {
		score += 220;
	}
	if (board.getBlackKing().get_bit(g8) || board.getBlackKing().get_bit(c8)) {
		score -= 220;
	}

	if (board.getCastlingRights() & 3)
		score += 80;
	if (board.getCastlingRights() & 12)
		score += 80;

	return score;
}

int Evaluate::rewardPawnShield(Board &board) const {
	int score = 0;

	// White castled kingside
	if (board.getWhiteKing().get_bit(g1)) {
		if (board.getWhitePawns().get_bit(f2))
			score += 45;
		if (board.getWhitePawns().get_bit(g2))
			score += 60; // most important
		if (board.getWhitePawns().get_bit(h2))
			score += 35;
	}

	// White castled queenside
	if (board.getWhiteKing().get_bit(c1)) {
		if (board.getWhitePawns().get_bit(b2))
			score += 40;
		if (board.getWhitePawns().get_bit(c2))
			score += 50;
		if (board.getWhitePawns().get_bit(d2))
			score += 35;
	}

	// Black castled kingside
	if (board.getBlackKing().get_bit(g8)) {
		if (board.getBlackPawns().get_bit(f7))
			score -= 45;
		if (board.getBlackPawns().get_bit(g7))
			score -= 60;
		if (board.getBlackPawns().get_bit(h7))
			score -= 35;
	}

	// Black castled queenside
	if (board.getBlackKing().get_bit(c8)) {
		if (board.getBlackPawns().get_bit(b7))
			score -= 40;
		if (board.getBlackPawns().get_bit(c7))
			score -= 50;
		if (board.getBlackPawns().get_bit(d7))
			score -= 35;
	}

	return score;
}

int Evaluate::rewardPawnAdvancement(Board &board) const {

	int score = 0;
	Bitboard whitePawns = board.getWhitePawns();
	Bitboard blackPawns = board.getBlackPawns();				// Reward pawns on rank 4 and 5 much more
	score += (whitePawns & 0x00000000FF000000ULL).count() * 40; // rank 4
	score += (whitePawns & 0x000000FF00000000ULL).count() * 60; // rank 5

	score -= (blackPawns & 0x00000000FF000000ULL).count() * 40;
	score -= (blackPawns & 0x000000FF00000000ULL).count() * 60;

	if (board.getWhitePawns().get_bit(e4) || board.getWhitePawns().get_bit(d4)) {
		score += 45;
	}
	if (board.getBlackPawns().get_bit(e5) || board.getBlackPawns().get_bit(d5)) {
		score -= 45;
	}
	return score;
}

int Evaluate::rewardKnightDevelopment(Board &board) const {
	int score = 0;

	if (board.getWhiteKnights().get_bit(a1) || board.getWhiteKnights().get_bit(h1) || board.getWhiteKnights().get_bit(a2) || board.getWhiteKnights().get_bit(h2) ||
		board.getWhiteKnights().get_bit(a3) || board.getWhiteKnights().get_bit(h3)) {
		score -= 35;
	}

	if (board.getBlackKnights().get_bit(a8) || board.getBlackKnights().get_bit(h8) || board.getBlackKnights().get_bit(a7) || board.getBlackKnights().get_bit(h7) ||
		board.getBlackKnights().get_bit(a6) || board.getBlackKnights().get_bit(h6)) {
		score += 35;
	}

	return score;
}

int Evaluate::rewardEndgameProgress(Board &board) const {
	int score = 0;

	Bitboard whitePawns = board.getWhitePawns();
	Bitboard blackPawns = board.getBlackPawns();

	// Strong reward for advanced pawns
	// Rank 4
	score += (whitePawns & 0x00000000FF000000ULL).count() * 35;
	score -= (blackPawns & 0x00000000FF000000ULL).count() * 35;

	// Rank 5
	score += (whitePawns & 0x000000FF00000000ULL).count() * 70;
	score -= (blackPawns & 0x000000FF00000000ULL).count() * 70;

	// Rank 6
	score += (whitePawns & 0x0000FF0000000000ULL).count() * 140;
	score -= (blackPawns & 0x0000FF0000000000ULL).count() * 140;

	// Rank 7 (Promotion)
	score += (whitePawns & 0x00FF000000000000ULL).count() * 250;
	score -= (blackPawns & 0x00FF000000000000ULL).count() * 250;

	// Reward king centralization in endgame
	int whiteKingSq = -1;
	int blackKingSq = -1;

	// Safely get king squares
	Bitboard wk = board.getWhiteKing();
	whiteKingSq = wk.pop_lsb();

	Bitboard bk = board.getBlackKing();
	blackKingSq = bk.pop_lsb();

	score += centerBonus(whiteKingSq);
	score -= centerBonus(blackKingSq);

	return score;
}

int Evaluate::centerBonus(int sq) const {
	if (sq == -1)
		return 0;
	int file = sq % 8;
	int rank = sq / 8;
	int dist = abs(file - 3) + abs(rank - 3);
	return (4 - dist) * 15; // higher bonus in center
}

void Evaluate::addPieceSquareScores(Bitboard pieces, const int *mgTable, const int *egTable, bool isWhite, int &mg, int &eg) {
	int sq;
	while ((sq = pieces.pop_lsb()) != -1) {
		int idx = isWhite ? sq : mirror(sq); // mirror square for Black

		if (isWhite) {
			mg += mgTable[idx];
			eg += egTable[idx];
		} else {
			mg -= mgTable[idx];
			eg -= mgTable[idx];
		}
	}
}
