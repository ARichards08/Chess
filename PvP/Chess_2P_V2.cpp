// Packages
#include <iostream>
#include <string>
#include <vector>

// Global Variables

// Board
std::vector<int> Board(64, 0);

// Colour that is currently to move, can be updated by assigning Piece::Colour
// Need a fuction that will update both the colour to move and the enemy colour at the same time
int ColourtoMove=1, ColourEnemy=0;



// Emplace back is trying to create a vector of objects but doesn't have a deafult constructor for that, need to work out how to push_nback/emplace_back a vector of objects to a vector of objects





// Classes

// Move class, just holds a initial square and a target square
class Move{
public:
    int initial_square;
    int target_square;

    // Default constructor
    Move(int initial, int target){
        initial_square=initial;
        target_square=target;
    };

    // Copy constructor
    Move(const Move &in_move){
        initial_square=in_move.initial_square;
        target_square=in_move.target_square;
    };

};


// Namespaces

namespace Piece{

    // Pieces are given an int value, 0 for none, 1-6 for the black pieces and 7-12 for the white pieces
    // Figure Enumeration
    enum Figure{None, Pawn, Rook, Knight, Bishop, Queen, King};

    // Colour Enumeration
    enum Colour{Black, White};

    // Function to add a piece to a square, returns the piece's int value
     int Add(Colour Col, Figure Fig){
        return Col*6+Fig;
    };

    // Function to remove a piece from a square, improves readability
     int Remove(){
        return None;
    };

    // Function returning a boolean on whether a piece is the chosen colour
    bool IsColour(int pc, int Col){
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
    bool IsFigure (int pc, int figure){
        // Account for Black and White colours
        if (pc == figure || pc == figure+6){
            return true;
        } else{
            return false;
        };
    };

    // Function returning a boolean on whether the piece is a sliding piece i.e. Rook, Bishop or Queen
    // They will share similar logic for moves
    bool IsSlidingPiece(int pc){
        if (IsFigure(pc, Rook) || IsFigure(pc, Bishop) || IsFigure(pc, Queen)){
            return true;
        } else {
            return false;
        }
    };
};


// Namespace with all the functions related to validating a legal move
namespace LegalMoves{

    // Direction offset constants to move sliding pieces around the board
    // const vectors aren't allowed, arrays only it seems
    // North, South, East, West, NE, SW, SE, NW
    const static std::vector<int> SlidingDirections={8, -8, 1, -1, 9, -9, -7, 7};

    // Static vector that contains the number of squares in 8 directions
    // Precomputing function will be called to set this up.
    std::vector<std::vector<int>> PrecomputingData(){
        std::vector<int> SingleSq;
        std::vector<std::vector<int>> result;
        int North, South, East, West, NE, SW, SE, NW;
        for (int rank=0; rank<8; rank++){
            for (int file=0; file<8; file++){
                North=7-rank;
                South=rank;
                East=7-file;
                West=file;
                NE=std::min(North, East);
                SW=std::min(South, West);
                SE=std::min(South, East);
                NW=std::min(North, West);
                SingleSq={North, South, East, West, NE, SW, SE, NW};

                result.push_back(SingleSq);
            };
        };
        return result;
    };

    const static std::vector<std::vector<int>> SquaresToEdge(PrecomputingData());

    // Function to generate the sliding piece moves
    // use insert to add the sliding moves to the move list
    std::vector<Move> GenerateSlidingMoves(int startSq){
        std::vector<Move> SlidingMovesList;
        int pc, targetSq, targetPc;

        pc=Board[startSq];

        // Start and End direction indices for rooks and bishops, queens search all 8 directions

        int startDirIndex=0, endDirIndex=8;
        if (Piece::IsFigure(targetPc, Piece::Figure::Bishop)) startDirIndex=4;
        if (Piece::IsFigure(targetPc, Piece::Figure::Rook)) endDirIndex=4;

        for (int directionId=startDirIndex; directionId<endDirIndex; directionId++){
            for (int n=0; n<SquaresToEdge[startSq][directionId]; n++){

                targetSq=startSq+SlidingDirections[directionId]*(n+1);
                targetPc=Board[targetSq];

                if (Piece::IsColour(targetPc, ColourtoMove)) break; // Break on reaching a square with friendly colour

                SlidingMovesList.push_back(Move(startSq, targetSq));

                if (Piece::IsColour(targetPc, ColourEnemy)) break; // Stop looking in that direction after capturing an enemy piece
            };
        };

        return SlidingMovesList;
    };

    std::vector<Move> GenerateKnightMoves(int startSq){
        std::vector<Move> KnightMovesList;
        std::vector<int> diagonals(2);
        int targetSq, targetPc, temp1, temp2;

        // Look in the cardinal directions 1 square first and then diagonals 
        for (int directionId=0; directionId<4; directionId++){

            // Determining the diagonal ids for each cardinal direction
            switch (directionId){
            case 0:
                diagonals={4, 7};
                break;
            case 1:
                diagonals={5, 6};
                break;
            case 2:
                diagonals={4, 6};
                break;
            case 3:
                diagonals={5, 7};
                break;            
            default:
                break;
            };

            // If each cardinal direction followed by either diagonal is within SquaresToEdge, set as targetSq
            if (SquaresToEdge[startSq][directionId] > 0){
                for (int i=0; i<2; i++){
                    temp1=startSq+SlidingDirections[directionId];
                    temp2=SquaresToEdge[temp1][diagonals[i]];
                    if (temp2 > 0){ // seg fault??

                        targetSq=startSq+SlidingDirections[directionId]+SlidingDirections[diagonals[i]];
                        targetPc=Board[targetSq];

                        // Break on friendly piece but if piece is enemy or none, add move to list
                        if (Piece::IsColour(targetPc, ColourtoMove)){
                            continue;
                        } else{
                            KnightMovesList.push_back(Move(startSq, targetSq));
                        };
                    };
                };
            };
        };
        
        return KnightMovesList;
    };


    // Function to calculate the number of squares to generate a list of legal moves at each position
    // Need to keep track of pinned pieces and discovered checks within smaller vectors, which are then used to generate the legal move list for the current player
    // Special moves: Pawn charge, En Passant, Castling, Promotion
    
    std::vector<Move> GenerateMoves(){
        std::vector<Move> MovesList, TempMoves;

        for (int startSq=0; startSq<64; startSq++){
            // Check the colour of the piece on the square is the colour to play
            if (Piece::IsColour(Board[startSq], ColourtoMove)){
                // Check which type of piece it is
                // Rook, Bishop or Queen
                if (Piece::IsSlidingPiece(Board[startSq])) {
                    TempMoves=GenerateSlidingMoves(startSq);
                    MovesList.insert(MovesList.end(), TempMoves.begin(), TempMoves.end());
                    TempMoves.clear();
                // Knight
                } else if (Piece::IsFigure(Board[startSq], Piece::Figure::Knight)){
                    TempMoves=GenerateKnightMoves(startSq);
                    MovesList.insert(MovesList.end(), TempMoves.begin(), TempMoves.end());
                    TempMoves.clear();
                // Empty
                } else {
                    continue;
                };
            };
        };

        return MovesList;
    };

};

// Move notation, if two same pieces can move to the same position, the file is added before the end position. If they are in the same file, the rank is added instead
// Rd8 > Rdd8 or R3d8 for moving a rook

// Functions

// Function to setup the chess board at the start of the program
void Setup_Board(){
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
std::string Reference(int Position){
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
void Print_Board(){
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

// Function to check if an inputted move uses two squares on the board, syntax check
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

// Function to convert an array index into a square reference
std::string Ind2Ref(int Ind){
    std::string letter;

    switch (Ind%8){
    case 0:
        letter='a';
        break;
    case 1:
        letter='b';
        break;
    case 2:
        letter='c';
        break;
    case 3:
        letter='d';
        break;
    case 4:
        letter='e';
        break;
    case 5:
        letter='f';
        break;
    case 6:
        letter='g';
        break;
    case 7:
        letter='h';
        break;
    default:
        break;
    };

    return letter+std::to_string(Ind/8 + 1);
};

// Function to move a piece to a square, from a square index i.e. 40 -> 56
void MakeMove(int initial, int target){
    Board[target]=Board[initial];
    Board[initial]=Piece::Remove();
};

// Function to move a piece to a square, from a square name i.e. a5c8
void MakeMove(std::string input){
    int initial, target;

    initial=Ref2Idx(input.substr(0, 2)), target=Ref2Idx(input.substr(2, 2));

    Board[target]=Board[initial];
    Board[initial]=Piece::Remove();
};

// Function to take a player's input to move a piece
void Player_move(std::string &input_move){
    // First check syntax
    input_move_syntax(input_move);

    // Then perform move
    MakeMove(input_move);
};

//////////
// Program
//////////

int main (){


// Setup Board
Setup_Board();

// Move list
std::vector<Move> MoveList;

MoveList=LegalMoves::GenerateMoves();
for (int i=0; i<MoveList.size(); i++){
    std::cout << Ind2Ref(MoveList[i].initial_square) << " " << Ind2Ref(MoveList[i].target_square) << " " << MoveList[i].initial_square << " " << MoveList[i].target_square << std::endl;
};

// Test stuff

// Player input
std::string White_Move, Black_Move;

// Program loop
Print_Board();

std::cout << "White player, Please enter your move, from initial square to target square i.e. a4c3: ";

//getline(std::cin, White_Move);

//Player_move(White_Move);

//Print_Board();

}