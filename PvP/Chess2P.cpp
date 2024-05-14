// Packages
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // find_if
#include <map> // Loading from FEN

// Class headers
#include "Move.h"
#include "Piece.h"

// Function headers
#include "LegalMoves.h"
#include "BoardFunctions.h"


// Global Variable Definitions

// Board
std::vector<int> Board(64, 0), PieceSquares;

// Colour that is currently to move, can be updated by assigning Piece::Colour
int ColourToMove=1, ColourEnemy=0;

// Array of the last move of each side, updated after each move
// Only needed for En Passant, but could display it for players as well
std::vector<Move> lastMoves(2);

//////////
// Program
//////////

int main (){


// Setup Board
SetupBoard();

// Move list
std::vector<Move> MoveList;

// Test stuff

//ClearBoard();
//SetupBoard("r3k3/1p3p2/p2q2p1/bn3P2/1N2PQP1/PB6/3K1R1r/3R4 w q - 0 1");


// Player input
std::string playerMove;

// Program loop
Print_Board();

std::cout << "Moves are input as the starting square followed by the target square i.e. a4c3. Castling is done by moving the King to the castled square followed by a C i.e. e8g8C. Promotion moves include a fifth character at the end, Q, B, K, R, i.e a7a8Q." << std::endl;

while (true){

    MoveList=LegalMoves::GenerateMoves();

    // If MoveList is empty, quit program, checkmates and draws are handled in the movelist creation
    if (MoveList.empty()) break;

    // Get a move from the player
    if (ColourToMove==Piece::Colour::White){
        std::cout << "White player move: ";
    } else {
        std::cout << "Black player move: ";
    };
    getline(std::cin, playerMove);

    attemptedMove(playerMove, MoveList);
    Print_Board(Piece::Colour::White); // Print board from white's perspective, flips board each turn by default
};

}