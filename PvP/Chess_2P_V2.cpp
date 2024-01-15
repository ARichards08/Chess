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

// Function to check if an inputted move is valid
void input_move_syntax(std::string &str){
    bool let_dig_check=false, length_check=false, dup_check=false;
    int sum;

    while (let_dig_check==false || length_check==false || dup_check==false){
        length_check=false, let_dig_check=false, dup_check=false;
        // Inputted move must be four characters long
        if (str.length()==4) length_check=true;

        // First two and last two characters must be a letter a-h and a number 1-8
        sum=0;
        for (int i=0; i<2; i++){
            if (str[2*i]=='a' || str[2*i]=='b' || str[2*i]=='c' || str[2*i]=='d' || str[2*i]=='e' || str[2*i]=='f' || str[2*i]=='g' || str[2*i]=='h') sum++;

            if (isdigit(str[2*i+1]) && (str[2*i+1]!='0' && str[2*i+1]!='9')) sum++;
        };
        if (sum==4) let_dig_check=true;

        if (str.substr(0, 2) != str.substr(2, 2)) dup_check=true;

        // Get a new move entered by the player if the one entered was invalid
        if (length_check==false){
            std::cout << "Please ensure that the length of your move is 4 characters long: ";
            getline(std::cin, str);
        } else if (let_dig_check==false) {
            std::cout << "Please ensure that the files are lowercase letters a-h and the rows are integers 1-8: ";
            getline(std::cin, str);
        } else if (dup_check==false) {
            std::cout << "Please ensure that the target square is not the same as the initial square: ";
            getline(std::cin, str);
        };
    };
};

// Function to convert a square reference into an array index
int Ref2Idx(std::string Ref){
    int i, j;

    switch (Ref[0]){
    case 'a':
        i=0;
        break;
    case 'b':
        i=1;
        break;
    case 'c':
        i=2;
        break;
    case 'd':
        i=3;
        break;
    case 'e':
        i=4;
        break;
    case 'f':
        i=5;
        break;
    case 'g':
        i=6;
        break;
    case 'h':
        i=7;
        break;
    default:
        i=0;
        break;
    };

    j=stoi(Ref.substr(1, 1))-1;
    return j*8+i;
};

// Function to move a piece to a square, from a square index i.e. 40 -> 56
void Move(int initial, int target, std::vector<unsigned short int> &Board){
    Board[target]=Board[initial];
    Board[initial]=Piece::Remove();
};

// Function to move a piece to a square, from a square name i.e. a5c8
void Move(std::string input, std::vector<unsigned short int> &Board){
    int initial, target;

    initial=Ref2Idx(input.substr(0, 2)), target=Ref2Idx(input.substr(2, 2));

    Board[target]=Board[initial];
    Board[initial]=Piece::Remove();
};

// Function to take a player's input to move a piece
void Player_move(std::string &input_move, std::vector<unsigned short int> &Board){
    // First check syntax
    input_move_syntax(input_move);

    // Then perform move
    Move(input_move, Board);
};

// Program

int main (int argc, char *argv[]){

// Board
std::vector<unsigned short int> Board(64, 0);

Setup_Board(Board);

// Player input
std::string White_Move, Black_Move;

// Program loop
Print_Board(Board);

std::cout << "White player, Please enter your move, from initial square to target square i.e. a4c3: ";

getline(std::cin, White_Move);

Player_move(White_Move, Board);

Print_Board(Board);

}