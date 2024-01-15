// Packages
#include <iostream>
#include <string>
#include <vector>

// Enumerations




// Namespaces
namespace Piece{

    // Pieces are given an int value, 0 for none, 1-6 for the black pieces and 7-12 for the white pieces
    // Figure Enumeration
    enum Figure{None, Pawn, Rook, Knight, Bishop, Queen, King};

    // Colour Enumeration
    enum Colour{Black, White};

    // Function to add a piece to a square, returns the piece's int value
    // Colour default is so that a colour doesn't have to be specified when lookign at empty places
    unsigned short int Add(Colour Col, Figure Fig){
        return Col*6+Fig;
    };

    // Function to remove a piece from a square, improves readability
    unsigned short int Remove(){
        return None;
    };
};

// Classes



// Move notation, if two same pieces can move to the same position, the file is added before the end position. If they are in the same file, the rank is added instead
// Rd8 > Rdd8 or R3d8 for moving a rook

// Functions

// Function to setup the chess board at the start of the program
void Setup_Board(std::vector<unsigned short int> &Board){
    // Add Row 1, White non-pawns
    Board[0]=Piece::Add(Piece::White, Piece::Rook), Board[1]=Piece::Add(Piece::White, Piece::Knight), Board[2]=Piece::Add(Piece::White, Piece::Bishop), Board[3]=Piece::Add(Piece::White, Piece::Queen), Board[4]=Piece::Add(Piece::White, Piece::King), Board[5]=Piece::Add(Piece::White, Piece::Bishop), Board[6]=Piece::Add(Piece::White, Piece::Knight), Board[7]=Piece::Add(Piece::White, Piece::Rook);
    // Add Row 2, White pawns
    for (int i=8*1; i<8*2; i++){
        Board[i]=Piece::Add(Piece::White, Piece::Pawn);
    };
    // Add empty Row 3-6
    for (int i=8*2; i<8*6; i++){
        Board[i]=Piece::Remove();
    };
    // Add Row 7, Black Pawns
    for (int i=8*6; i<8*7; i++){
        Board[i]=Piece::Add(Piece::Black, Piece::Pawn);
    };
    // Add Row 8, Black non-pawns
    Board[56]=Piece::Add(Piece::Black, Piece::Rook), Board[57]=Piece::Add(Piece::Black, Piece::Knight), Board[58]=Piece::Add(Piece::Black, Piece::Bishop), Board[59]=Piece::Add(Piece::Black, Piece::Queen), Board[60]=Piece::Add(Piece::Black, Piece::King), Board[61]=Piece::Add(Piece::Black, Piece::Bishop), Board[62]=Piece::Add(Piece::Black, Piece::Knight), Board[63]=Piece::Add(Piece::Black, Piece::Rook);
};

// Function to return 2 letters as a piece instead of an int, makes the board more readable before graphics are implemented
std::string Reference(unsigned short int Position){
    switch (Position){
    case Piece::White*6 + Piece::Pawn :
        return "wp";
        break;
    case Piece::White*6 + Piece::Rook :
        return "wr";
        break;
    case Piece::White*6 + Piece::Knight :
        return "wn";
        break;
    case Piece::White*6 + Piece::Bishop :
        return "wb";
        break;
    case Piece::White*6 + Piece::Queen :
        return "wq";
        break;
    case Piece::White*6 + Piece::King :
        return "wk";
        break;
    case Piece::Black*6 + Piece::Pawn :
        return "bp";
        break;
    case Piece::Black*6 + Piece::Rook :
        return "br";
        break;
    case Piece::Black*6 + Piece::Knight :
        return "bn";
        break;
    case Piece::Black*6 + Piece::Bishop :
        return "bb";
        break;
    case Piece::Black*6 + Piece::Queen :
        return "bq";
        break;
    case Piece::Black*6 + Piece::King :
        return "bk";
        break;
    default:
        return "--";
    };
};

// Function to print the board
void Print_Board(std::vector<unsigned short int> &Board){
    int k;

    // Files
    std::cout << "    a  b  c  d  e  f  g  h \n" << std::endl;

    for (int i=7; i>=0; i--){
        // Numbers
        std::cout << i+1 << "  ";

        for (int j=0; j<8; j++) {
            std::cout << Reference(Board[i*8+j]) << " ";
        };
        std::cout << std::endl;
    };
};

// Program

int main (int argc, char *argv[]){

// Board
std::vector<unsigned short int> Board(64, 0);

Setup_Board(Board);
Print_Board(Board);

}