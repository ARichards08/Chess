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

// Variable to hold if there is a valid enpassant target file, equal to -1 if there is none
extern int EnPassantFile;

// Keeps track of each sides castling rights
// First index is the colour to move, second index is the queen or king side. This is easily shown in the global enum
extern std::vector<std::vector<bool>> CastlingRights;
enum CastlingSide{Queenside, Kingside};

// Vector to hold the squares the king occupys
extern std::vector<int> KingSquares;

// Game History held within a vector, might be better to use something like a queue but I'm not familiar enough with these things as the moment
// The game history is stored as a series of zobrist keys
extern std::vector<unsigned long long> GameHistory;

#endif