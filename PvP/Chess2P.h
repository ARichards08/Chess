#ifndef CHESS2P_H
#define CHESS2P_H

#include <vector>
#include "Move.h"

// Global Variable Declarations
// Defined in Chess.cpp

// Board
extern std::vector<int> Board, PieceSquares;

// Colour that is currently to move, can be updated by assigning Piece::Colour
extern int ColourToMove, ColourEnemy;

// Array of the last move of each side, updated after each move
// Only needed for En Passant, but could display it for players as well
extern std::vector<Move> lastMoves;

// Keeps track of each sides castling rights
extern std::vector<bool> CastleQueenside, CastleKingside;

// Vector to hold the squares the king occupys
extern std::vector<int> KingSquares;


#endif