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

	// Final score
	return material + positional;
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
