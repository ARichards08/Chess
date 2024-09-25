#include "BoardFunctions.h"

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <cmath>

#include "Chess2P.h"
#include "Piece.h"
#include "Move.h"
//#include "LegalMoves.h"
#include "Zobrist.h"

// Function to return a unicode character based on a square number. Unicode figures for all chess pieces and black and white squares
// Annoyingly the colours are inverted if the terminal is set to dark mode. Will use a seperate function to print dark mode characters as the opposite colour
std::string Reference(int Position){
    switch (Board[Position]){
    case Piece::White*6 + Piece::Pawn :
        return "\u2659";
        break;
    case Piece::White*6 + Piece::Rook :
        return "\u2656";
        break;
    case Piece::White*6 + Piece::Knight :
            return "\u2658";
        break;
    case Piece::White*6 + Piece::Bishop :
        return "\u2657";
        break;
    case Piece::White*6 + Piece::Queen :
        return "\u2655";
        break;
    case Piece::White*6 + Piece::King :
        return "\u2654";
        break;
    case Piece::Black*6 + Piece::Pawn :
        return "\u265F";
        break;
    case Piece::Black*6 + Piece::Rook :
        return "\u265C";
        break;
    case Piece::Black*6 + Piece::Knight :
        return "\u265E";
        break;
    case Piece::Black*6 + Piece::Bishop :
        return "\u265D";
        break;
    case Piece::Black*6 + Piece::Queen :
        return "\u265B";
        break;
    case Piece::Black*6 + Piece::King :
        return "\u265A";
        break;
    default:

        int column=Position%8;
        int row=(Position-column)/8;
        if ((row+column)%2==0){
            // Black Square
            return "\u25A0";
        } else {
            // White Square
            return "\u25A1";
        };
    };
};

std::string ReferenceDarkMode(int Position){
    switch (Board[Position]){
    case Piece::White*6 + Piece::Pawn :
        return "\u265F";
        break;
    case Piece::White*6 + Piece::Rook :
        return "\u265C";
        break;
    case Piece::White*6 + Piece::Knight :
            return "\u265E";
        break;
    case Piece::White*6 + Piece::Bishop :
        return "\u265D";
        break;
    case Piece::White*6 + Piece::Queen :
        return "\u265B";
        break;
    case Piece::White*6 + Piece::King :
        return "\u265A";
        break;
    case Piece::Black*6 + Piece::Pawn :
        return "\u2659";
        break;
    case Piece::Black*6 + Piece::Rook :
        return "\u2656";
        break;
    case Piece::Black*6 + Piece::Knight :
        return "\u2658";
        break;
    case Piece::Black*6 + Piece::Bishop :
        return "\u2657";
        break;
    case Piece::Black*6 + Piece::Queen :
        return "\u2655";
        break;
    case Piece::Black*6 + Piece::King :
        return "\u2654";
        break;
    default:

        int column=Position%8;
        int row=(Position-column)/8;
        if ((row+column)%2==0){
            // Black Square
            return "\u25A1";
        } else {
            // White Square
            return "\u25A0";
        };

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

    j=std::stoi(Ref.substr(1, 1))-1;
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

// Function to return the row of a square on the board
// Numbered 0-7 for indexing
int Row(int square){
    return std::floor(square/8);
};

// Function to return the file of a square on the board
// Numbered 0-7 for indexing
int File(int square){
    return square%8;
};

// Function for setting up the board from a FEN string, default argument is the initial board position. Doesn't take the Halfmove clock and Fullmove number information
void SetupBoard(std::string FENstring){
    char piece;
    int square=56, empty;
    std::string row, FENboard, FENinfo;
    std::size_t pre=0;
    Piece::Colour PieceColour;
    Piece::Figure PieceType;

    // Map of lowercase characters in FEN to piece types
    std::map<char, Piece::Figure> PieceFromSymbol{
        {'k', Piece::Figure::King}, {'q', Piece::Figure::Queen}, {'b', Piece::Figure::Bishop},
        {'n', Piece::Figure::Knight}, {'r', Piece::Figure::Rook}, {'p', Piece::Figure::Pawn}
    };

    // Split the FEN string to just have the piece positions in FENboard, and the rest in FENinfo
    auto it=FENstring.find(' ');
    FENboard=FENstring.substr(0, it), FENinfo=FENstring.substr(++it, FENstring.length());

    // Positions from FENboard
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

    // Information from FENinfo

    // Colour to move
    if (FENinfo[0]=='w'){
        ColourToMove=Piece::Colour::White, ColourEnemy=Piece::Colour::Black;
    } else {
        ColourToMove=Piece::Colour::Black, ColourEnemy=Piece::Colour::White;
    };

    // Castles avaliable
    it=FENinfo.find(' ');

    CastlingRights.assign(2, std::vector<bool>(2, false));
    
    auto it2=FENinfo.find(' ', ++it);

    while (it!=it2){
        if (FENinfo[it] == 'K'){
            CastlingRights[Piece::Colour::White][CastlingSide::Kingside]=true;
        } else if (FENinfo[it] == 'Q'){
            CastlingRights[Piece::Colour::White][CastlingSide::Queenside]=true;
        } else if (FENinfo[it] == 'k'){
            CastlingRights[Piece::Colour::Black][CastlingSide::Kingside]=true;
        } else if (FENinfo[it] == 'q'){
            CastlingRights[Piece::Colour::Black][CastlingSide::Queenside]=true;
        };

        it++;
    };
    it++;

    // En Passant avaliable
    if (FENinfo[it] !='-'){
        std::string str=FENinfo.substr(it, 2);
        int EPsq=Ref2Idx(str);

        EnPassantFile=File(EPsq);
    } else {
        EnPassantFile=-1;
    };

    // Update KingSquares
    KingSquares[ColourToMove]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourToMove));});;
    KingSquares[ColourEnemy]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourEnemy));});;

};

// Function to clear the board
void ClearBoard(){
    for (int i=0; i<Board.size(); i++){
        Piece::Remove(i);
    };
};

// Function to print the board
void Print_Board(int Perspective/*=ColourToMove*/){

    if (Perspective==Piece::Colour::White){
        // Files
        std::cout << "    a b c d e f g h " << std::endl;

        for (int i=7; i>=0; i--){
            // Numbers
            std::cout << i+1 << "  ";

            for (int j=0; j<8; j++) {
                std::cout << ReferenceDarkMode(i*8+j) << " ";
            };
            std::cout << std::endl;
        };
    } else {
        // Files
        std::cout << "    h g f e d c b a " << std::endl;
    
        for (int i=0; i<=7; i++){
            // Numbers
            std::cout << i+1 << "  ";

            for (int j=7; j>=0; j--) {
                std::cout << ReferenceDarkMode(i*8+j) << " ";
            };
            std::cout << std::endl;
        };
    };
};

// Function to move a piece to a square, from a square index i.e. 40 -> 56
void MakeMove(Move updateMove){

    Piece::Figure movingPieceType=Piece::ReturnFigure(Board[updateMove.initial_square]);

    int oldEnPassantFile=EnPassantFile;
    EnPassantFile=-1; // EnPassant file is changed to -1 here, but later is given another value if the next move is a pawn charge

    std::vector<std::vector<bool>> oldCastlingRights=CastlingRights;

    // Can be used to check for captures, if =0, then not a capture, except for en passant as this is always a capture
    Piece::Figure capturedPieceType=Piece::ReturnFigure(Board[updateMove.target_square]);
    bool isCapture = capturedPieceType!=Piece::Figure::None;


    // Always removing the moving piece from it's starting square
    Piece::Remove(updateMove.initial_square);
    Zobrist::UpdatePieceZobristKey(updateMove.initial_square, ColourToMove, movingPieceType);

    // If the move is a capture, remove enemy piece at target square
    if (isCapture && updateMove.flag!=Move::MFlag::EnPassant) Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourEnemy, capturedPieceType);

    // Use a switch statement here for the flags of the different moves
    switch (updateMove.flag)
    {

    // Castling
    case Move::MFlag::Castling :
        // Move King
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::King);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::King);

        // Castling left
        if (updateMove.target_square < updateMove.initial_square){
            Piece::Add(updateMove.target_square+1, ColourToMove, Piece::Figure::Rook);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square+1, ColourToMove, Piece::Figure::Rook);
            Piece::Remove(updateMove.target_square-1);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square-1, ColourToMove, Piece::Figure::Rook);

        // Castling right
        } else{
            Piece::Add(updateMove.target_square-1, ColourToMove, Piece::Figure::Rook);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square-1, ColourToMove, Piece::Figure::Rook);
            Piece::Remove(updateMove.target_square+1);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square+1, ColourToMove, Piece::Figure::Rook);
        };

        break;

    // En Passant
    case Move::MFlag::EnPassant :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Pawn);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::Pawn);

        // Determine whether or not to remove the piece above or below the target square depending on the colour
        if (ColourToMove==Piece::Colour::White){
            Piece::Remove(updateMove.target_square-8);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square-8, ColourEnemy, Piece::Figure::Pawn);
        } else {
            Piece::Remove(updateMove.target_square+8);
            Zobrist::UpdatePieceZobristKey(updateMove.target_square+8, ColourEnemy, Piece::Figure::Pawn);
        };
    
        break;

    // Pawn Promotions
    case Move::MFlag::PromoteRook :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Rook);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::Rook);

        break;

    case Move::MFlag::PromoteKnight :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Knight);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::Knight);

        break;

    case Move::MFlag::PromoteBishop :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Bishop);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::Bishop);

        break;

    case Move::MFlag::PromoteQueen :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::Figure::Queen);
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, Piece::Figure::Queen);

        break;

    // Pawn Charge
    // Update EnPassant File along with the regular move
    case Move::MFlag::PawnCharge :
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(movingPieceType));
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, movingPieceType);

        EnPassantFile=File(updateMove.target_square);

        break;

    default:
        // Regular move
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(movingPieceType));
        Zobrist::UpdatePieceZobristKey(updateMove.target_square, ColourToMove, movingPieceType);

        break;
    };

    // Update KingSquare array if the initial square was the king square
    if (updateMove.initial_square == KingSquares[ColourToMove]) KingSquares[ColourToMove]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourToMove));});;

    // Castling checks after possible castling moves

    // Enemy Checks
    if (CastlingRights[ColourEnemy][CastlingSide::Queenside]==true || CastlingRights[ColourEnemy][CastlingSide::Kingside]==true){
        // Disables enemy castling right on rook capture
        if (isCapture && capturedPieceType==Piece::Rook && Row(updateMove.target_square)==(ColourToMove==1 ? 7 : 0)){
            if (File(updateMove.target_square)==0) CastlingRights[ColourEnemy][CastlingSide::Queenside]=false;
            if (File(updateMove.target_square)==7) CastlingRights[ColourEnemy][CastlingSide::Kingside]=false;
        };
    };

    // Friendly checks
    if (CastlingRights[ColourToMove][CastlingSide::Queenside]==true || CastlingRights[ColourToMove][CastlingSide::Kingside]==true){
        // Disables friendly castling right on rook move
        if (movingPieceType==Piece::Rook && Row(updateMove.initial_square)== (ColourToMove==1 ? 0 : 7)){
            if (File(updateMove.initial_square)==0) CastlingRights[ColourToMove][CastlingSide::Queenside]=false;
            if (File(updateMove.initial_square)==7) CastlingRights[ColourToMove][CastlingSide::Kingside]=false;
        };
        
        // Disables both friendly castling rights if king moves
        if (movingPieceType==Piece::King){
            CastlingRights[ColourToMove][CastlingSide::Kingside]=false;
            CastlingRights[ColourToMove][CastlingSide::Queenside]=false;
        };
    };

    // Update Zobrist Castling Rights, if they have changed
    if (oldCastlingRights != CastlingRights) Zobrist::UpdateCastlingZobristKey(oldCastlingRights, CastlingRights);

    // Update Zobrist EnPassant File
    if (oldEnPassantFile!=EnPassantFile) Zobrist::UpdateEnPassantZobristKey(oldEnPassantFile, EnPassantFile);

    // Finally, swap ColourToMove and EnemyColour
    std::swap(ColourToMove, ColourEnemy);
    Zobrist::UpdateSideToMoveZobristKey();
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