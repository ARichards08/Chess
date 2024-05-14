#ifndef PIECE_H
#define PIECE_H

namespace Piece{

    // Pieces are given an int value, 0 for none, 1-6 for the black pieces and 7-12 for the white pieces
    // Figure Enumeration
    enum Figure{None, Pawn, Rook, Knight, Bishop, Queen, King};

    // Colour Enumeration
    enum Colour{Black, White};

    // Function to add a piece to a square, returns the piece's int value
    void Add(int sq, int Col, Figure Fig);

    // Function to remove a piece from a square, improves readability
    void Remove(int sq);

    // Function returning a boolean on whether a piece is the chosen colour
    bool IsColour(int pc, int Col);

    // Function returning a boolean on whether a piece is the chosen Figure
    bool IsFigure (int pc, int figure);

    // Function returning a boolean on whether the piece is a sliding piece i.e. Rook, Bishop or Queen
    // They will share similar logic for moves
    bool IsSlidingPiece(int pc);

    // Function to return the figure of a piece on a given square
    Figure ReturnFigure(int pc);
};

#endif