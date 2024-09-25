#ifndef BOARDFUNCTIONS_H
#define BOARDFUNCTIONS_H

#include <vector>
#include <string>
#include "Chess2P.h"
#include "Move.h"

// Function to return a unicode character based on a square number. Unicode figures for all chess pieces and black and white squares
// Annoyingly the colours are inverted if the terminal is set to dark mode. Will use a seperate function to print dark mode characters as the opposite colour
std::string Reference(int Position);
std::string ReferenceDarkMode(int Position);

// Function to check if an inputted move uses two squares on the board, syntax check
void input_move_syntax(std::string &str);

// Function to convert a square reference into an array index
int Ref2Idx(std::string Ref);

// Function to convert an array index into a square reference
std::string Ind2Ref(int Ind);

// Function to return the row of a board square
int Row(int square);

// Function to return the column of a board square
int File(int square);

// Function for setting up the board from a FEN string, default argument is the initial board position
void SetupBoard(std::string FENstring="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

// Function to check for the 50 move rule draw
bool HalfmoveCheck();

// Function to clear the board
void ClearBoard();

// Function to print the board
void Print_Board(int Perspective=ColourToMove);

// Function to move a piece to a square, from a square index i.e. 40 -> 56
void MakeMove(Move updateMove);

// Function to search a list of moves, looking for a match for an initial and target square. If pawn promotion, checks for promo flags
Move FindMove(Move inMove, std::vector<Move> MoveList);

// Function to take a player's input to move a piece
void attemptedMove(std::string &input, std::vector<Move> &MoveList);


#endif