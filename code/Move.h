#pragma once

struct Move {
    int from_square;        // Source square (0-63)
    int to_square;          // Destination square (0-63)
    bool is_promotion;      // Flag for pawn promotion (piece type to be handled later)
    bool is_capture;        // Flag for capture moves
    bool is_castle;         // Flag for castling
    bool is_en_passant;     // Flag for en passant

    // Default constructor
    Move() : from_square(-1), to_square(-1), is_promotion(false),
             is_capture(false), is_castle(false), is_en_passant(false) {}

    // Constructor with parameters
    Move(int from, int to, bool prom = false,
         bool capture = false, bool castle = false, bool en_passant = false)
        : from_square(from), to_square(to), is_promotion(prom),
          is_capture(capture), is_castle(castle), is_en_passant(en_passant) {}
};

