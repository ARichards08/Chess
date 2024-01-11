// Packages
#include <iostream>
#include <string>
#include <vector>

// Classes

// Piece abstract base class
// legal moves, legal captures?


// Move notation, if two same pieces can move to the same position, the file is added before the end position. If they are in the same file, the rank is added instead
// Rd8 > Rdd8 or R3d8 for moving a rook


// Functions

// Function to enumerate every type of piece. 0 represents no piece on the square
enum {
    wp=1,
    wr,
    wn,
    wb,
    wq,
    wk,
    bp,
    br,
    bn,
    bb,
    bq,
    bk
};

// Function to return the value of the enum from the number
std::string str_piece(int value){
    switch (value){
    case wp:
        return "wp";
        break;
    case wr:
        return "wr";
        break;
    case wn:
        return "wn";
        break;
    case wb:
        return "wb";
        break;
    case wq:
        return "wq";
        break;
    case wk:
        return "wk";
        break;
    case bp:
        return "bp";
        break;
    case br:
        return "br";
        break;
    case bn:
        return "bn";
        break;
    case bb:
        return "bb";
        break;
    case bq:
        return "bq";
        break;
    case bk:
        return "bk";
        break;
    default:
        return "--";
        break;
    }
};

// Function to print the board
void print_board(std::vector<std::vector<int>> board){
    for (int i=7; i>=0; i--){
        for (int j=0; j<8; j++){
            std::cout << str_piece(board[i][j]) << " ";
        };
        std::cout << std::endl;
    };
};

// Program

int main (){

std::vector<std::vector<int>> board;

// Setup board
board={
{wr, wn, wb, wq, wk, wb, wn, wr},
std::vector<int>(8, wp),
std::vector<int>(8, 0),
std::vector<int>(8, 0),
std::vector<int>(8, 0),
std::vector<int>(8, 0),
std::vector<int>(8, bp),
{br, bn, bb, bq, bk, bb, bn, br}
};


print_board(board);
    
}