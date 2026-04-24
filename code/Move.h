#pragma once

struct Move {
	int from_square = -1;
	int to_square = -1;
	int is_promotion = 0; // 1=q, 2=n, 3=b, 4=r
	bool is_capture = false;
	bool is_castle = false;
	bool is_en_passant = false;

	// Undo information
	int capturedPiece = -1;
	int oldCastlingRights = 0;
	int oldEnPassantSquare = -1;

	// Default constructor
	Move() = default;

	// Constructor for creating moves
	Move(int from, int to, int prom = 0, bool capture = false, bool castle = false, bool en_passant = false)
		: from_square(from), to_square(to), is_promotion(prom), is_capture(capture), is_castle(castle), is_en_passant(en_passant) {}
};
