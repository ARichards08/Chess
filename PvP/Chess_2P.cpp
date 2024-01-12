// Packages
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// Enumerations

// Enumerate colour
// Blank is for empty squares
enum colour{
    white,
    black,
    blank
};

// Enumerate every file
enum file{a, b, c, d, e, f, g, h};

// Enumerate every type of piece. em represents an empty sqaure, w and b are white and black, and the other letters are the pieces
enum representation{em, wp, wr, wn, wb, wq, wk, bp, br, bn, bb, bq, bk};

// Classes

// Piece abstract base class
// legal moves, legal captures?

// Abstract base class Piece holds the variables and virtual function names they all will need
class Pieces{
    public:
        colour piece_colour;
        int piece_rank;
        file piece_file;
        representation rep;

        virtual void set_representation(colour piece_colour)=0;

        // Constructor
        Pieces(colour c, int r, file f){piece_colour=c, piece_rank=r, piece_file=f;};
};

// Empty class
class Empty: public Pieces{
    public:

    void set_representation(colour piece_colour){
        rep=em;
    };

    // Constructor
    Empty(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour=blank);};
};

// Pawn class
class Pawn: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wp;
        } else {
            rep=bp;
        }
    };

    // Constructor
    Pawn(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};

// Rook Class
class Rook: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wr;
        } else {
            rep=br;
        }
    };

    // Constructor
    Rook(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};

// Knight Class
class Knight: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wn;
        } else {
            rep=bn;
        }
    };

    // Constructor
    Knight(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};

// Bishop Class
class Bishop: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wb;
        } else {
            rep=bb;
        }
    };

    // Constructor
    Bishop(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};


// Queen Class
class Queen: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wq;
        } else {
            rep=bq;
        }
    };

    // Constructor
    Queen(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};

// King Class
class King: public Pieces{
    public:

    void set_representation(colour piece_colour){
        if (piece_colour==white){
            rep=wk;
        } else {
            rep=bk;
        }
    };

    // Constructor
    King(colour c, int r, file f): Pieces(c, r, f){set_representation(piece_colour);};
};

// Move notation, if two same pieces can move to the same position, the file is added before the end position. If they are in the same file, the rank is added instead
// Rd8 > Rdd8 or R3d8 for moving a rook


// Functions

// Function to return the value of the enum from the number
std::string str_piece(representation value){
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
    case em:
        return "--";
        break;
    default:
        return "Err";
        break;
    };
};

// Function to set up the board
// Need to use static_cast to get the value of an enum by index: https://stackoverflow.com/questions/321801/enum-c-get-by-index
void setup_board(std::vector<std::vector<Pieces*>> &board){
    // Clear whatever was previously on the board
    board.clear();

    // Row vector
    std::vector<Pieces*> row;

    for (int i=0; i<8; i++){
        // White bottom row
        if (i==0){
            row={new Rook(white, i, a), new Knight(white, i, b), new Bishop(white, i, c), new Queen(white, i, d), new King(white, i, e), new Bishop(white, i, f), new Knight(white, i, g), new Rook(white, i, h)};
        }
        // White Pawn row
        else if (i==1) {
            for(int j=file::a; j<=file::h; j++){
                row.push_back(new Pawn(white, i, static_cast<file>(j)));
            };
        }
        // Black Pawn row
        else if (i==6) {
            for(int j=file::a; j<=file::h; j++){
                row.push_back(new Pawn(black, i, static_cast<file>(j)));
            };
        }
        // Black bottom row
        else if (i==7) {
            row={new Rook(black, i, a), new Knight(black, i, b), new Bishop(black, i, c), new Queen(black, i, d), new King(black, i, e), new Bishop(black, i, f), new Knight(black, i, g), new Rook(black, i, h)};
        }
        // Empty rows
        else{
            for(int j=file::a; j<=file::h; j++){
                row.push_back(new Empty(blank, i, static_cast<file>(j)));
            };
        };

        // Add row to board and clear row
        board.push_back(row);
        row.clear();
    };
};

// Function to print the board
void print_board(std::vector<std::vector<Pieces*>> &board){
    for (int i=7; i>=0; i--){
        for (int j=0; j<8; j++){
            std::cout << str_piece(board[i][j]->rep) << " ";
        };
        std::cout << std::endl;
    };
};

// Function to print square
void print_square(std::string square, std::vector<std::vector<Pieces*>> board){

    // Map file letters to integers
    std::unordered_map<char, int> files={{'a', 0}, {'b', 1}, {'c', 2}, {'d', 3}, {'e', 4}, {'f', 5}, {'g', 6}, {'h', 7}};

    // stoi input has to be a string and a digit apparantly, can't be a character or a string with letters
    std::cout << str_piece(board[stoi(square.substr(1, 1))-1][files[square[0]]]->rep) << std::endl;
};

// Program

int main (){

// Setup board
std::vector<std::vector<Pieces*>> board;
setup_board(board);

// Print board
print_board(board);
    
// Print square
print_square("d8", board);
}