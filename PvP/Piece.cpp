#include "Piece.h"

#include "Chess2P.h"
#include <algorithm>

// Function to add a piece to a square, returns the piece's int value
void Piece::Add(int sq, int Col, Figure Fig){
    Board[sq]=Col*6+Fig;

    // If the square doesn't already have a piece on it, add it to the PieceSquares array
    if (std::find(std::begin(PieceSquares), std::end(PieceSquares), sq) == std::end(PieceSquares)) PieceSquares.push_back(sq);
};

// Function to remove a piece from a square, improves readability
void Piece::Remove(int sq){

    Board[sq]=None;

    auto sq_it=std::find(std::begin(PieceSquares), std::end(PieceSquares), sq);

    // Remove square from PieceSquare array
    if (sq_it != std::end(PieceSquares)) PieceSquares.erase(sq_it);
};

// Function returning a boolean on whether a piece is the chosen colour
bool Piece::IsColour(int pc, int Col){
    if (Col==Colour::Black){
        if (pc>=1 && pc<=6){
            return true;
        } else{
            return false;
        };
    } else if (Col==Colour::White){
        if (pc>=7 && pc<=12){
            return true;
        } else{
            return false;
        };
    };
    // If the square is empty

    return false;
};

// Function returning a boolean on whether a piece is the chosen Figure
bool Piece::IsFigure (int pc, int figure){
    // Account for Black and White colours
    if (pc > 6) pc-=6;

    if (pc == figure){
        return true;
    } else{
        return false;
    };
};

// Function returning a boolean on whether the piece is a sliding piece i.e. Rook, Bishop or Queen
// They will share similar logic for moves
bool Piece::IsSlidingPiece(int pc){
    if (IsFigure(pc, Rook) || IsFigure(pc, Bishop) || IsFigure(pc, Queen)){
        return true;
    } else {
        return false;
    }
};

// Function to return the figure of a piece on a given square
Piece::Figure Piece::ReturnFigure(int pc){
    if (pc > 6) pc-=6;
    return (Piece::Figure) pc; // Cast to enum
};