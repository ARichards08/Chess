// Packages
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // find_if
#include <map> // Loading from FEN

// Global Variables

// Board
std::vector<int> Board(64, 0), PieceSquares;

// Colour that is currently to move, can be updated by assigning Piece::Colour
int ColourToMove=1, ColourEnemy=0;



// Classes

// Move class, holds a initial square, a target square and a flag
class Move{
public:
    int initial_square;
    int target_square;
    int flag;
    // Flag values:
    //

    enum MFlag{None, Castling, EnPassant, PawnCharge, PromoteRook, PromoteKnight, PromoteBishop, PromoteQueen};

    // Default constructor
    Move(int initial=0, int target=0, MFlag f=None){
        initial_square=initial;
        target_square=target;
        flag=f;
    };

    // Copy constructor
    Move(const Move &in_move){
        initial_square=in_move.initial_square;
        target_square=in_move.target_square;
        flag=in_move.flag;
    };

    // Constructor for a move and a new flag
    Move(const Move &in_move, MFlag f){
        initial_square=in_move.initial_square;
        target_square=in_move.target_square;
        flag=f;
    };

};

// Array of the last move of each side, updated after each move
// Only needed for En Passant, but could display it for players as well
std::vector<Move> lastMoves(2);


// Namespaces

namespace Piece{

    // Pieces are given an int value, 0 for none, 1-6 for the black pieces and 7-12 for the white pieces
    // Figure Enumeration
    enum Figure{None, Pawn, Rook, Knight, Bishop, Queen, King};

    // Colour Enumeration
    enum Colour{Black, White};

    // Function to add a piece to a square, returns the piece's int value
    void Add(int sq, int Col, Figure Fig){
        Board[sq]=Col*6+Fig;

        // If the square doesn't already have a piece on it, add it to the PieceSquares array
        if (std::find(std::begin(PieceSquares), std::end(PieceSquares), sq) == std::end(PieceSquares)) PieceSquares.push_back(sq);
    };

    // Function to remove a piece from a square, improves readability
    void Remove(int sq){

        Board[sq]=None;

        auto sq_it=std::find(std::begin(PieceSquares), std::end(PieceSquares), sq);

        // Remove square from PieceSquare array
        if (sq_it != std::end(PieceSquares)) PieceSquares.erase(sq_it);
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
        if (pc > 6) pc-=6;

        if (pc == figure){
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

    // Function to return the figure of a piece on a given square
    Figure ReturnFigure(int pc){
        if (pc > 6) pc-=6;
        return (Figure) pc; // Cast to enum
    }
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





    // Function to generate the pseudo-legal sliding piece moves
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

                if (Piece::IsColour(targetPc, ColourToMove)) break; // Break on reaching a square with friendly colour

                SlidingMovesList.push_back(Move(startSq, targetSq));

                if (Piece::IsColour(targetPc, ColourEnemy)) break; // Stop looking in that direction after capturing an enemy piece
            };
        };

        return SlidingMovesList;
    };

    // Function to generate the pseudo-legal Knight moves
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
                        if (Piece::IsColour(targetPc, ColourToMove)){
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

    // Array of two boolean constants, [0] for black and [1] for white, same as the colours. Set to true initially, if King or Rooks are moved its set to false
    std::vector<bool> leftCastle(2, true), rightCastle(2, true);

    // Function to generate the pseudo-legal King moves
    // Will include Castling
    std::vector<Move> GenerateKingMoves(int startSq){
        std::vector<Move> KingMovesList;
        int targetSq, targetPc;

        // Regular Moves
        for (int directionId=0; directionId<8; directionId++){
            if (SquaresToEdge[startSq][directionId] > 0){

                targetSq=startSq+SlidingDirections[directionId];
                targetPc=Board[targetSq];

                // Break on reaching a square with friendly colour
                if (Piece::IsColour(targetPc, ColourToMove)){
                    continue; 
                } else{
                    KingMovesList.push_back(Move(startSq, targetSq));
                };
            };
        };

        // Castling, only saves the king move

        // If neither King nor left Rook has moved
        if (leftCastle[ColourToMove]==true){
            // If the three squares between the left Rook and the King are empty, add the move
            if (Piece::IsFigure(Board[startSq-1], Piece::Figure::None) && Piece::IsFigure(Board[startSq-2], Piece::Figure::None) && Piece::IsFigure(Board[startSq-3], Piece::Figure::None)){
                // Add King move
                KingMovesList.push_back(Move(startSq, startSq-3, Move::MFlag::Castling));
            };
        };
        // Same again but for right
        if (leftCastle[ColourToMove]==true){
            // If the two squares between the right Rook and the King are empty, add the move
            if (Piece::IsFigure(Board[startSq+1], Piece::Figure::None) && Piece::IsFigure(Board[startSq+2], Piece::Figure::None)){
                // Add King move
                KingMovesList.push_back(Move(startSq, startSq+2, Move::MFlag::Castling));
            };
        };

        return KingMovesList;
    };

    // Function to check if a pawn's move will allow it to promote, and adds possible moves to the PawnMovesList
    void PromoteCheck(Move PawnMove, std::vector<Move>& List){
        // Check for Pawn promotion
        if (PawnMove.target_square > 55 || PawnMove.target_square < 8){
            // If a pawn is moving the the first or last rows, add the move with each of the promotion flags
            List.push_back(Move(PawnMove, Move::MFlag::PromoteRook));
            List.push_back(Move(PawnMove, Move::MFlag::PromoteKnight));
            List.push_back(Move(PawnMove, Move::MFlag::PromoteBishop));
            List.push_back(Move(PawnMove, Move::MFlag::PromoteQueen));
        };
    };

    // Function to generate the pseudo-legal Pawn moves
    // No need to check for promotions on charging and en passant moves
    std::vector<Move> GeneratePawnMoves(int startSq){
        std::vector<Move> PawnMovesList;
        std::vector<int> diagonals(2);
        int targetSq, targetPc;

        // Only move N, NE or NW when the player colour is white
        if (ColourToMove==Piece::Colour::White){
            // Forward
            if (SquaresToEdge[startSq][0] > 0){
                targetSq=startSq+SlidingDirections[0];
                targetPc=Board[targetSq];

                // Check the forward square is empty
                if (Piece::IsFigure(targetPc, Piece::Figure::None)){
                    PromoteCheck(Move(startSq, targetSq), PawnMovesList);
                    PawnMovesList.push_back(Move(startSq, targetSq));
                };
            };

            // Charging
            // Check if the pawn is in the second row from it's side, if so allow it to charge
            if (startSq > 7 && startSq < 16){
                targetSq=startSq+SlidingDirections[0]*2;
                targetPc=Board[targetSq];

                // Check the forward square is empty
                if (Piece::IsFigure(targetPc, Piece::Figure::None)){
                    PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::PawnCharge));
                };
            };

            // Attacking
            diagonals={4, 7}; // NE and NW

            for (int i=0; i<2; i++){
                if (SquaresToEdge[startSq][diagonals[i]] > 0){
                    targetSq=startSq+SlidingDirections[diagonals[i]];
                    targetPc=Board[targetSq];

                    // Check that the piece in the diagonal square is an enemy piece
                    if (Piece::IsColour(targetPc, ColourEnemy)){
                        PromoteCheck(Move(startSq, targetSq), PawnMovesList);
                        PawnMovesList.push_back(Move(startSq, targetSq));
                    };

                    // En Passant check
                    // Check if the last enemy move was a pawn charge to a square on either side of the current pawn
                    // Can't promote on an en passant move so no check
                    // Have to check the board edges as well
                    if (lastMoves[ColourEnemy].flag == Move::MFlag::PawnCharge){
                        if ((lastMoves[ColourEnemy].target_square == startSq-1 && i == 1 && SquaresToEdge[startSq][7] > 0) || (lastMoves[ColourEnemy].target_square == startSq+1 && i==0 && SquaresToEdge[startSq][4] > 0)){
                            PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::EnPassant));
                        };
                    };

                };
            };

        // Only move S, SE or SW when the player colour is black
        } else{
            // Forward
            if (SquaresToEdge[startSq][1] > 0){
                targetSq=startSq+SlidingDirections[1];
                targetPc=Board[targetSq];
                
                // Check the forward square is empty
                if (Piece::IsFigure(targetPc, Piece::Figure::None)){
                    PromoteCheck(Move(startSq, targetSq), PawnMovesList);
                    PawnMovesList.push_back(Move(startSq, targetSq));
                };
            };

            // Charging
            // Check if the pawn is in the second row from it's side, if so allow it to charge
            if (startSq > 47 && startSq < 56){
                targetSq=startSq+SlidingDirections[1]*2;
                targetPc=Board[targetSq];

                // Check the forward square is empty
                if (Piece::IsFigure(targetPc, Piece::Figure::None)){
                    PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::PawnCharge));
                };
            };

            // Attacking
            diagonals={5, 6}; // SW and SE

            for (int i=0; i<2; i++){
                if (SquaresToEdge[startSq][diagonals[i]] > 0){
                    targetSq=startSq+SlidingDirections[diagonals[i]];
                    targetPc=Board[targetSq];

                    // Check that the piece in the diagonal square is an enemy piece
                    if (Piece::IsColour(targetPc, ColourEnemy)){

                        PromoteCheck(Move(startSq, targetSq), PawnMovesList);
                        PawnMovesList.push_back(Move(startSq, targetSq));
                    };

                    // En Passant check
                    // Check if the last enemy move was a pawn charge to a square on either side of the current pawn 
                    // Can't promote on an en passant move so no check
                    if (lastMoves[ColourEnemy].flag == Move::MFlag::PawnCharge){
                        if ((lastMoves[ColourEnemy].target_square == startSq-1 && i == 0 && SquaresToEdge[startSq][5] > 0) || (lastMoves[ColourEnemy].target_square == startSq+1 && i==1 && SquaresToEdge[startSq][6] > 0)){
                            PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::EnPassant));
                        };
                    };

                };
            };
        };

        return PawnMovesList;
    };


    // Function to calculate the number of squares to generate a list of legal moves at each position
    // Need to keep track of pinned pieces and discovered checks within smaller vectors, which are then used to generate the legal move list for the current player
    std::vector<Move> GenerateMoves(){
        std::vector<Move> MovesList, TempMoves;
        int startSq;

        // Iterate over the PieceSquares array instead of the whole board
        for (int sq=0; sq<PieceSquares.size(); sq++){
            startSq=PieceSquares[sq];
            // Check the colour of the piece on the square is the colour to play
            if (Piece::IsColour(Board[startSq], ColourToMove)){
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
                // King
                } else if (Piece::IsFigure(Board[startSq], Piece::Figure::King)){
                    TempMoves=GenerateKingMoves(startSq);
                    MovesList.insert(MovesList.end(), TempMoves.begin(), TempMoves.end());
                    TempMoves.clear();
                // Pawn
                } else if (Piece::IsFigure(Board[startSq], Piece::Figure::Pawn)){
                    TempMoves=GeneratePawnMoves(startSq);
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

// Function for setting up the board from a FEN string, default argument is the initial board position
void SetupBoard(std::string FENstring="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){
    char piece;
    int square=56, empty;
    std::string row, FENboard;
    std::size_t pre=0;
    Piece::Colour PieceColour;
    Piece::Figure PieceType;

    // Map of lowercase characters in FEN to piece types
    std::map<char, Piece::Figure> PieceFromSymbol{
        {'k', Piece::Figure::King}, {'q', Piece::Figure::Queen}, {'b', Piece::Figure::Bishop},
        {'n', Piece::Figure::Knight}, {'r', Piece::Figure::Rook}, {'p', Piece::Figure::Pawn}
    };

    // Split the FEN string to just have the piece positions
    FENboard=FENstring.substr(0, FENstring.find(' '));

    for (int i=0; i<8; i++){
        // Extract each row from the FENboard
        std::size_t post=FENboard.find('/', pre);
        if (post==std::string::npos) post=FENboard.length();

        row=FENboard.substr(pre, post-pre);
        //std::cout << row << '|' << std::endl;

        // Iterate through each character in the row string, filling in squares
        for (int j=0; j<row.length(); j++){

            // If the character is a digit, add that many consequective empty squares
            if (isdigit(row[j])){
                empty=row[j] - '0'; // Cast char to int by subtracting '0'
                while (empty > 0){
                    Piece::Remove(square);
                    square++;
                    empty--;
                };
            } else {

                // Find colour from the case of the character, uppercase is white, lowercase is black
                if (isupper(row[j])){
                    PieceColour=Piece::Colour::White;
                } else {
                    PieceColour=Piece::Colour::Black;
                };

                PieceType=PieceFromSymbol[tolower(row[j])];

                Piece::Add(square, PieceColour, PieceType);
                square++;
            };
        };
        // Move down a rank
        square=square-16;

        pre=post+1;
    };
};

// Function to clear the board
void ClearBoard(){
    for (int i=0; i<Board.size(); i++){
        Piece::Remove(i);
    };
}

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
    bool let_dig_check=false, length_check=false, dup_check=false, promo_check=true;
    int sum;

    while (let_dig_check==false || length_check==false || dup_check==false || promo_check==false){
        length_check=false, let_dig_check=false, dup_check=false, promo_check=true;
        // Inputted normal moves must be four characters long
        if (str.length()==4 ) length_check=true;

        // Promotion moves are 5 characters long, only need to check the fifth character if the length is 5
        if (str.length()==5){
            length_check=true;
            if (str[4]!='Q' && str[4]!='B' && str[4]!='K' && str[4]!='R') promo_check=false;
        };

        // First two and last two characters must be a letter a-h and a number 1-8
        sum=0;
        for (int i=0; i<2; i++){
            if (str[2*i]=='a' || str[2*i]=='b' || str[2*i]=='c' || str[2*i]=='d' || str[2*i]=='e' || str[2*i]=='f' || str[2*i]=='g' || str[2*i]=='h') sum++;

            if (isdigit(str[2*i+1]) && (str[2*i+1]!='0' && str[2*i+1]!='9')) sum++;
        };
        if (sum==4) let_dig_check=true;

        // Checking the move isn't moving to the same square
        if (str.substr(0, 2) != str.substr(2, 2)) dup_check=true;

        // Get a new move entered by the player if the one entered was invalid
        if (length_check==false){
            std::cout << "Please ensure that the length of your move is 4 characters long (5 for castling and promotion): ";
            getline(std::cin, str);
        } else if (let_dig_check==false) {
            std::cout << "Please ensure that the files are lowercase letters a-h and the rows are integers 1-8: ";
            getline(std::cin, str);
        } else if (dup_check==false) {
            std::cout << "Please ensure that the target square is not the same as the initial square: ";
            getline(std::cin, str);
        } else if (promo_check==false){
            std::cout << " Please ensure that castling and promotion moves include the correct special character, Q, B, K, R for promotions, C for castling: ";
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
void MakeMove(Move updateMove){


    // Use a switch statement here for the flags of the different moves
    switch (updateMove.flag)
    {
    case Move::MFlag::Castling :
        // Move King
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(Board[updateMove.initial_square]));
        //Board[updateMove.target_square]=Board[updateMove.initial_square];
        Piece::Remove(updateMove.initial_square);

        // Castling left
        if (updateMove.target_square < updateMove.initial_square){
            Piece::Add(updateMove.target_square+1, ColourToMove, Piece::Figure::Rook);
            Piece::Remove(updateMove.target_square-1);

        // Castling right
        } else{
            Piece::Add(updateMove.target_square-1, ColourToMove, Piece::Figure::Rook);
            Piece::Remove(updateMove.target_square+1);
        };

        break;

    // En Passant
    case Move::MFlag::EnPassant :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(Board[updateMove.initial_square]));
        //Board[updateMove.target_square]=Board[updateMove.initial_square];
        Piece::Remove(updateMove.initial_square);

        // Determine whether or not to remove the piece above or below the target square depending on the colour
        if (ColourToMove==Piece::Colour::White){
            Piece::Remove(updateMove.target_square-8);
        } else {
            Piece::Remove(updateMove.target_square+8);
        };
    
        break;

    // Pawn Promotions
    case Move::MFlag::PromoteRook :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Rook);
        Piece::Remove(updateMove.initial_square);

        break;

    case Move::MFlag::PromoteKnight :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Knight);
        Piece::Remove(updateMove.initial_square);

        break;

    case Move::MFlag::PromoteBishop :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Bishop);
        Piece::Remove(updateMove.initial_square);

        break;

    case Move::MFlag::PromoteQueen :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Queen);
        Piece::Remove(updateMove.initial_square);

        break;

    default:
        // Regular move
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(Board[updateMove.initial_square]));
        //Board[updateMove.target_square]=Board[updateMove.initial_square];
        Piece::Remove(updateMove.initial_square);
        break;
    }
    

    // Update the lastMoves array
    lastMoves[ColourToMove]=updateMove;


    // Castling checks after possible castling moves

    // Updates anytime the king moves in the 5th file
    if (Piece::IsFigure(Board[updateMove.initial_square], Piece::King)){
        if (updateMove.initial_square%8 == 4){
            LegalMoves::leftCastle[ColourToMove]=false;
            LegalMoves::rightCastle[ColourToMove]=false;
        };

    // Updates anytime a rook moves from the edge files
    } else if (Piece::IsFigure(Board[updateMove.initial_square], Piece::Rook)){
        // Left file
        if (updateMove.initial_square%8 == 0){
            LegalMoves::leftCastle[ColourToMove]=false;
        // Right file
        } else if (updateMove.initial_square%8 == 7){
            LegalMoves::rightCastle[ColourToMove]=false;
        };
    };


    // Finally, swap ColourToMove and EnemyColour
    std::swap(ColourToMove, ColourEnemy);
};

// Function to search a list of moves, looking for a match for an initial and target square. If pawn promotion, checks for promo flags
Move FindMove(Move inMove, std::vector<Move> MoveList){
    std::vector<Move>::iterator it;

    // inMove either has flag=None or a promotion flag
    if (inMove.flag==Move::MFlag::None){
        it=std::find_if(MoveList.begin(), MoveList.end(), [&inMove](Move i){
            if (inMove.initial_square==i.initial_square && inMove.target_square==i.target_square){
                return true;
            } else {
                return false;
            };
        });
    // Promotion moves
    } else {
        it=std::find_if(MoveList.begin(), MoveList.end(), [&inMove](Move i){
            if (inMove.initial_square==i.initial_square && inMove.target_square==i.target_square && inMove.flag==i.flag){
                return true;
            } else {
                return false;
            };
        });
    };

    if (it==MoveList.end()) return Move(0, 0);
    return *it;
};

// Function to take a player's input to move a piece
void attemptedMove(std::string &input, std::vector<Move> &MoveList){
    int initial, target;
    Move::MFlag flag;
    Move finalMove;
    std::string reenteredInput;

    // First check syntax
    input_move_syntax(input);

    // Check for the move in the MoveList
    initial=Ref2Idx(input.substr(0, 2)), target=Ref2Idx(input.substr(2, 2)), flag=Move::MFlag::None;

    // Only need to specify flags for promotions
    if (input.length()==5){
        if (input[4]=='Q'){
            flag=Move::MFlag::PromoteQueen;
        } else if (input[4]=='B'){
            flag=Move::MFlag::PromoteBishop;
        } else if (input[4]=='K'){
            flag=Move::MFlag::PromoteKnight;
        } else if (input[4]=='R'){
            flag=Move::MFlag::PromoteRook;
        };
    };

    Move searchMove(initial, target, flag);

    // Find move in the MoveList
    finalMove=FindMove(searchMove, MoveList);

    // If the move isn't in the MoveList it returns a1a1, which correpsonds to an illegal move
    // Otherwise, perform the move that was found
    if (!(finalMove.initial_square==0 && finalMove.target_square==0)){
        MakeMove(finalMove);
    } else {
        std::cout << "Illegal move, please reenter your move: ";
        getline(std::cin, reenteredInput);
        attemptedMove(reenteredInput, MoveList);
    };

};

//////////
// Program
//////////

int main (){


// Setup Board
SetupBoard();

// Move list
std::vector<Move> MoveList;

// Test stuff

//for (int i=0; i<PieceSquares.size(); i++){
//    std::cout << PieceSquares[i] << std::endl;
//};

//MoveList=LegalMoves::GenerateMoves();
//for (int i=0; i<MoveList.size(); i++){
//    std::cout << Ind2Ref(MoveList[i].initial_square) << " " << Ind2Ref(MoveList[i].target_square) << std::endl;
//};
//std::cout << "Number of moves: " << MoveList.size() << std::endl;


// Player input
std::string White_Move, Black_Move;

// Program loop
Print_Board();

std::cout << "Moves are input as the starting square followed by the target square i.e. a4c3. Castling is done by moving the King to the castled square followed by a C i.e. e8g8C. Promotion moves include a fifth character at the end, Q, B, K, R, i.e a7a8Q." << std::endl;

while (false){
    std::cout << "White player move: ";
    getline(std::cin, White_Move);
    MoveList=LegalMoves::GenerateMoves();
    attemptedMove(White_Move, MoveList);
    Print_Board();

    std::cout << "Black player move: ";
    getline(std::cin, Black_Move);
    MoveList=LegalMoves::GenerateMoves();
    attemptedMove(Black_Move, MoveList);
    Print_Board();
};

//getline(std::cin, White_Move);

//attempetedMove(White_Move);

//Print_Board();

}