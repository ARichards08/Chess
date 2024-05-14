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

    // Setup Knight diagonals for each cardinal direction
    // North, South, East, West
    const static std::vector<std::vector<int>> diagonals={{4, 7}, {5, 6}, {4, 6}, {5, 7}};

    // Array of two boolean constants, [0] for black and [1] for white, same as the colours. Set to true initially, if King or Rooks are moved its set to false
    std::vector<bool> CastleQueenside(2, true), CastleKingside(2, true);

    // Array that will hold the square numbers that a piece is allowed to move to to block check from a sliding piece
    std::vector<int> SquaresToBlock;

    // Vector to hold the squares the king occupys
    static std::vector<int> KingSquares(2, 0);

    // Array of the last move of each side, updated after each move
    // Only needed for En Passant, but could display it for players as well
    std::vector<Move> lastMoves(2);

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

    // Function to generate the sliding attacks that the opponent can make on the current move
    void GenerateSlidingAttacks(int startSq, std::vector<int>& SquaresAttacked){
        int pc, targetSq, targetPc;

        pc=Board[startSq];

        // Start and End direction indices for rooks and bishops, queens search all 8 directions
        int startDirIndex=0, endDirIndex=8;
        if (Piece::IsFigure(pc, Piece::Figure::Bishop)) startDirIndex=4;
        if (Piece::IsFigure(pc, Piece::Figure::Rook)) endDirIndex=4;

        for (int directionId=startDirIndex; directionId<endDirIndex; directionId++){
            for (int n=0; n<SquaresToEdge[startSq][directionId]; n++){

                targetSq=startSq+SlidingDirections[directionId]*(n+1);
                targetPc=Board[targetSq];

                // Add square to array
                SquaresAttacked.push_back(targetSq);

                // If target square is the friendly King's square, add the previous squares to SquaresToBlock
                if (targetSq==KingSquares[ColourToMove]){
                    for (int i=0; i<=n; i++){
                        SquaresToBlock.push_back(startSq+SlidingDirections[directionId]*(i));
                    };
                };

                if (!Piece::IsFigure(targetPc, Piece::Figure::None)) break; // Stop looking in that direction if there is a piece of either colour there
            };
        };
    };

    // Function to generate the attacks that the opponent can make on the current move with knights
    void GenerateKnightAttacks(int startSq, std::vector<int>& SquaresAttacked){
        int targetSq, temp1, temp2;

        // Look in the cardinal directions 1 square first and then diagonals 
        for (int directionId=0; directionId<4; directionId++){

            // If each cardinal direction followed by either diagonal is within SquaresToEdge, set as targetSq
            if (SquaresToEdge[startSq][directionId] > 0){
                for (int i=0; i<2; i++){
                    temp1=startSq+SlidingDirections[directionId];
                    temp2=SquaresToEdge[temp1][diagonals[directionId][i]];
                    if (temp2 > 0){ //seg fault?? Tidy this up

                        targetSq=startSq+SlidingDirections[directionId]+SlidingDirections[diagonals[directionId][i]];


                        // Add square to array
                        SquaresAttacked.push_back(targetSq);

                        // If the target square is the friendly King's square, add the knight square to SquaresToBlock, so that it can be taken to stop check
                        if (targetSq==KingSquares[ColourToMove]) SquaresToBlock.push_back(startSq);
                    };
                };
            };
        };
    };

    // Function to generate the attacks that the opponent can make on the current move with pawns, not counting en passant captures
    void GeneratePawnAttacks(int startSq, std::vector<int>& SquaresAttacked){
        int targetSq, direction;

        // Establish direction of pawns as North or South based on colour. Use the diagonals array to find the directions for North with Id=0 and South with Id=1
        if (ColourEnemy == Piece::Colour::White){
            direction=0;
        } else {
            direction=1;
        };

        // Only care about the squares being attacked, so diagonal captures and en passant
        for (int i=0; i<2; i++){
            if (SquaresToEdge[startSq][diagonals[direction][i]] > 0){
                targetSq=startSq+SlidingDirections[diagonals[direction][i]];


                // Add square to array
                SquaresAttacked.push_back(targetSq);

                // If the target square is the friendly King's square, add the pawn square to SquaresToBlock, so that it can be taken to stop check
                if (targetSq==KingSquares[ColourToMove]) SquaresToBlock.push_back(startSq);

                // En Passant check
                // Checking the possibility of En Passant breaking a pin line, 11.30 first vid

            };
        };
    };

    // Function to generate the attacks that the opponent can make on the current move with the King, not taking into account how such a move would place the king in danger
    void GenerateKingAttacks(int startSq, std::vector<int>& SquaresAttacked){
        int targetSq;

        // Regular Moves
        for (int directionId=0; directionId<8; directionId++){
            if (SquaresToEdge[startSq][directionId] > 0){

                targetSq=startSq+SlidingDirections[directionId];

                // Add square to array
                SquaresAttacked.push_back(targetSq);
            };
        };
    };

    // Function to calculate the squares the opponent is currently attacking. This should only be used to calculate the King's moves, the pinned squares function is for other pieces
    // Doesn't include squares that are under attack from en passant following the current player move, this doesn't matter for Check but does for capture evals
    std::vector<int> SquaresUnderAttack(){
        std::vector<int> SquaresAttacked;
        int startSq;

        // Clear the SquaresToBlock vector from previous moves
        SquaresToBlock.clear();

        // Iterate over the PieceSquares array instead of the whole board
        for (int sq=0; sq<PieceSquares.size(); sq++){
            startSq=PieceSquares[sq];

            // Check the colour of the piece on the square is the colour to play
            if (Piece::IsColour(Board[startSq], ColourEnemy)){
                // Check which type of piece it is
                // Rook, Bishop or Queen
                if (Piece::IsSlidingPiece(Board[startSq])) {
                    GenerateSlidingAttacks(startSq, SquaresAttacked);
                };
                // Knight
                if (Piece::IsFigure(Board[startSq], Piece::Knight)) {
                    GenerateKnightAttacks(startSq, SquaresAttacked);
                };
                // Pawn
                if (Piece::IsFigure(Board[startSq], Piece::Pawn)) {
                    GeneratePawnAttacks(startSq, SquaresAttacked);
                };
                // King
                if (Piece::IsFigure(Board[startSq], Piece::King)){
                    GenerateKingAttacks(startSq, SquaresAttacked);
                };

                // Sort and erase duplicates, I think it's faster doing it in one go here than checking the array each time I add a square
                // Not going to do this, as I can use duplicates for check for double check situations

                //std::sort(std::begin(SquaresAttacked), std::end(SquaresAttacked));
                //SquaresAttacked.erase(std::unique(std::begin(SquaresAttacked), std::end(SquaresAttacked)), std::end(SquaresAttacked));
            };
        };
        return SquaresAttacked;
    };

    // Function to calculate a 2d vector, that contains the squares in each pinned ray on the board for the friendly colour
    std::vector<std::vector<int>> CalculatePinnedRays(const std::vector<int>& SquaresAttacked){
        std::vector<std::vector<int>> PinnedRays;
        std::vector<int> PinnedRay;
        int startSq=KingSquares[ColourToMove], targetSq, targetPc;
        bool foundPinned;
        Piece::Figure bishopOrRook;

        // Iterate over every direction from the king, and check if there is an enemy sliding piece there
        for (int directionId=0; directionId<8; directionId++){
            foundPinned=false;

            // Define this loop as a lambda expression so that it can easily be broken out of by returning
            [&](){

                for (int n=0; n<SquaresToEdge[startSq][directionId]; n++){
                    targetSq=startSq+SlidingDirections[directionId]*(n+1);
                    targetPc=Board[targetSq];

                    if (directionId<4){
                        bishopOrRook=Piece::Figure::Rook;
                    } else{
                        bishopOrRook=Piece::Figure::Bishop;
                    };


                    // If the target square piece is the enemy colour and can move along the ray
                    if (Piece::IsColour(targetPc, ColourEnemy) && (Piece::IsFigure(targetPc, Piece::Queen) || (Piece::IsFigure(targetPc, bishopOrRook)))){
                        
                        // Check that there is only a single friendly piece between the king and the attacking piece, also no enemy pieces
                        for (int i=1; i<n; i++){
                            targetSq=startSq+SlidingDirections[directionId]*(i);
                            targetPc=Board[targetSq];

                            // return if there is an enemy piece between the attacking piece and the king, to stop looking in this direction
                            if (Piece::IsColour(targetPc, ColourEnemy)) return;

                            // return if there are multiple friendly pieces in the space between the attacking piece and the friendly king
                            if (Piece::IsColour(targetPc, ColourToMove)){
                                if (foundPinned){
                                    return;
                                } else {
                                    foundPinned=true;
                                };
                            };
                        };

                        // If the code didn't return and foundPinned is true, there is only 1 friendly piece in the pin ray, so the ray is valid
                        for (int i=0; i<n; i++){
                            PinnedRay.push_back(startSq+SlidingDirections[directionId]*(i+1));
                        };

                        PinnedRays.push_back(PinnedRay);
                        PinnedRay.clear();
                    };
                };

            };
        };

        return PinnedRays;
    };

    // Function to return an iterator to the ray that a square is pinned in. If the square isn't pinned, it returns the end iterator of the PinnedRays vector
    std::vector<std::vector<int>>::iterator PinnedRayIt(const int& sq, std::vector<std::vector<int>>& PinnedRays){

        for (std::vector<std::vector<int>>::iterator it=std::begin(PinnedRays); it!=std::end(PinnedRays); ++it){
            auto foundPin=std::find(std::begin(*it), std::end(*it), sq);
            if (foundPin != std::end(*it)) return it;
        };

        return std::end(PinnedRays);
    };

    // Function to calculate if a pinned sliding piece is moving along the pinned ray
    // Note that dir = 0-7, the indices of SlidingDirections
    bool IsMovingAlongPinnedRay(const int& dir, const int& startSq){
        
        int actualDir=startSq-KingSquares[ColourToMove];

        if (actualDir % 9==0){
            actualDir=9;
        } else if(actualDir % 8==0){
            actualDir=8;
        } else if(actualDir % 7==0){
            actualDir=7;
        } else if(actualDir % 1==0){
            actualDir=1;
        };

        return (actualDir==SlidingDirections[dir] || actualDir==-SlidingDirections[dir]);
    };

    // Function to generate the pseudo-legal sliding piece moves
    // use insert to add the sliding moves to the move list
    void GenerateSlidingMoves(int startSq, std::vector<Move>& SlidingMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays){
        int pc=Board[startSq], targetSq, targetPc;

        // If piece is pinned and the king is in check, piece cannot move
        if (Check && PinnedRayIt(startSq, PinnedRays) != std::end(PinnedRays)) return;

        // Start and End direction indices for rooks and bishops, queens search all 8 directions

        int startDirIndex=0, endDirIndex=8;
        if (Piece::IsFigure(pc, Piece::Figure::Bishop)) startDirIndex=4;
        if (Piece::IsFigure(pc, Piece::Figure::Rook)) endDirIndex=4;

        for (int directionId=startDirIndex; directionId<endDirIndex; directionId++){
            for (int n=0; n<SquaresToEdge[startSq][directionId]; n++){

                // If the piece is pinned, ignore directions that aren't along the pinned ray
                if (PinnedRayIt(startSq, PinnedRays) != std::end(PinnedRays) && !IsMovingAlongPinnedRay(directionId, startSq)){
                    continue;
                };

                targetSq=startSq+SlidingDirections[directionId]*(n+1);
                targetPc=Board[targetSq];

                if (Piece::IsColour(targetPc, ColourToMove)) break; // Break on reaching a square with friendly colour

                // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                bool movePreventsCheck=std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock);
                if (!Check || movePreventsCheck){
                    SlidingMovesList.push_back(Move(startSq, targetSq));
                };

                // Stop looking in that direction after capturing an enemy piece
                // If the current move prevents check, subsequent moves won't
                if (Piece::IsColour(targetPc, ColourEnemy) || movePreventsCheck) break; 
            };
        };
    };

    // Function to generate the pseudo-legal Knight moves
    void GenerateKnightMoves(int startSq, std::vector<Move>& KnightMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays){
        int targetSq, targetPc, temp1, temp2;

        // Knight can't move if it is pinned
        if (PinnedRayIt(startSq, PinnedRays) != std::end(PinnedRays)) return;

        // Look in the cardinal directions 1 square first and then diagonals 
        for (int directionId=0; directionId<4; directionId++){

            // If each cardinal direction followed by either diagonal is within SquaresToEdge, set as targetSq
            if (SquaresToEdge[startSq][directionId] > 0){
                for (int i=0; i<2; i++){
                    temp1=startSq+SlidingDirections[directionId];
                    temp2=SquaresToEdge[temp1][diagonals[directionId][i]];
                    if (temp2 > 0){ //seg fault?? Tidy this up

                        targetSq=startSq+SlidingDirections[directionId]+SlidingDirections[diagonals[directionId][i]];
                        targetPc=Board[targetSq];

                        // Break on friendly piece but if piece is enemy or none, add move to list
                        if (Piece::IsColour(targetPc, ColourToMove)){
                            continue;
                        } else{
                            // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                            if (!Check || std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock)){
                                KnightMovesList.push_back(Move(startSq, targetSq));
                            };
                        };
                    };
                };
            };
        };
    };

    // Function to generate the pseudo-legal King moves
    // Will include Castling
    void GenerateKingMoves(int startSq, std::vector<Move>& KingMovesList, const bool& inCheck, const std::vector<int>& SquaresAttacked){
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
                    // Only add the move if the target square is not under attack
                    if (std::find(std::begin(SquaresAttacked), std::end(SquaresAttacked), targetSq) == std::end(SquaresAttacked)) KingMovesList.push_back(Move(startSq, targetSq));
                };
            };
        };

        // Castling, only saves the king move

        // Can't castle out of check
        if (!inCheck){
            // If neither King nor left Rook has moved
            if (CastleQueenside[ColourToMove]==true){
                // If the three squares between the left Rook and the King are empty, and the Rook square does have a rook, add the move
                if (Piece::IsFigure(Board[startSq-1], Piece::Figure::None) && Piece::IsFigure(Board[startSq-2], Piece::Figure::None) && Piece::IsFigure(Board[startSq-3], Piece::Figure::None) && Piece::IsFigure(Board[startSq-4], Piece::Figure::Rook)){
                    // Add King move if the target square is not under attack
                    if (std::find(std::begin(SquaresAttacked), std::end(SquaresAttacked), startSq-3) == std::end(SquaresAttacked)) KingMovesList.push_back(Move(startSq, startSq-3, Move::MFlag::Castling));
                };
            };
            // Same again but for right
            if (CastleKingside[ColourToMove]==true){
                // If the two squares between the right Rook and the King are empty, and the Rook square does have a rook, add the move
                if (Piece::IsFigure(Board[startSq+1], Piece::Figure::None) && Piece::IsFigure(Board[startSq+2], Piece::Figure::None) && Piece::IsFigure(Board[startSq+3], Piece::Figure::Rook)){
                    // Add King move if the target square is not under attack
                    if (std::find(std::begin(SquaresAttacked), std::end(SquaresAttacked), startSq+2) == std::end(SquaresAttacked)) KingMovesList.push_back(Move(startSq, startSq+2, Move::MFlag::Castling));
                };
            };
        };
    };

    // Function to check if a pawn's move will allow it to promote, and adds possible moves to the PawnMovesList
    void PromotionMoves(Move PawnMove, std::vector<Move>& List){
        // If a pawn is moving the the first or last rows, add the move with each of the promotion flags
        List.push_back(Move(PawnMove, Move::MFlag::PromoteRook));
        List.push_back(Move(PawnMove, Move::MFlag::PromoteKnight));
        List.push_back(Move(PawnMove, Move::MFlag::PromoteBishop));
        List.push_back(Move(PawnMove, Move::MFlag::PromoteQueen));
    };

    // Function to check if an En Passant move will reveal a check, by looking from the king in all directions
    bool InCheckAfterEP(const int & startSq, const int& epTargetSq){
        int targetSq, targetPc;
        Piece::Figure bishopOrRook;
        bool inCheck=false;

        // Make move
        Piece::Remove(startSq);
        Piece::Add(epTargetSq, ColourToMove, Piece::Figure::Pawn);

        if (ColourToMove==Piece::Colour::White){
            Piece::Remove(epTargetSq-8);
        } else{
            Piece::Remove(epTargetSq+8);
        };

        // See if King is in check

        // Iterate over every direction from the king, and check if there is an enemy sliding piece there
        for (int directionId=0; directionId<8; directionId++){

            // Define this loop as a lambda expression so that it can easily be broken out of by returning
            [&](){
                for (int n=0; n<SquaresToEdge[KingSquares[ColourToMove]][directionId]; n++){
                    targetSq=KingSquares[ColourToMove]+SlidingDirections[directionId]*(n+1);
                    targetPc=Board[targetSq];

                    if (directionId<4){
                        bishopOrRook=Piece::Figure::Rook;
                    } else{
                        bishopOrRook=Piece::Figure::Bishop;
                    };


                    // If the target square piece is the enemy colour and can move along the ray, inCheck=true and exit loop
                    if (Piece::IsColour(targetPc, ColourEnemy) && (Piece::IsFigure(targetPc, Piece::Queen) || (Piece::IsFigure(targetPc, bishopOrRook)))){
                        inCheck=true;
                        return;
                    // If the target square has any piece other than an attacking sliding piece, the king cannot be attacked from this direction, so skip to the next
                    } else if (!Piece::IsFigure(targetPc, Piece::Figure::None)){
                        return;
                    };
                };
            };

            // If the King is in check after EP, exit loop early
            if (inCheck) break;
        };

        // Unmake move and return inCheck

        Piece::Add(startSq, ColourToMove, Piece::Figure::Pawn);
        Piece::Remove(epTargetSq);
        
        if (ColourToMove==Piece::Colour::White){
            Piece::Add(epTargetSq-8, ColourEnemy, Piece::Figure::Pawn);
        } else{
            Piece::Add(epTargetSq+8, ColourEnemy, Piece::Figure::Pawn);
        };

        return inCheck;

    };

    // Function to generate the pseudo-legal Pawn moves
    // No need to check for promotions on charging and en passant moves
    void GeneratePawnMoves(int startSq, std::vector<Move>& PawnMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays){
        int targetSq, targetPc, forwardIt, diagonalIt, pawnRowStart, pawnRowEnd;

        // Promote if pawn is in either in the tope or bottom row
        bool nextStepPromote = startSq>55 || startSq<8;

        // Only move N, NE or NW when the player colour is white
        if (ColourToMove==Piece::Colour::White){
            forwardIt=0;
            diagonalIt=0;
            pawnRowStart=8;
            pawnRowEnd=15;
        // Only move S, SE or SW when the player colour is white
        } else {
            forwardIt=1;
            diagonalIt=1;
            pawnRowStart=48;
            pawnRowEnd=55;
        };

        // Only consider forward moves if pawn isn't pinned or if is moving along the pinned ray
        if (PinnedRayIt(startSq, PinnedRays) == std::end(PinnedRays) || IsMovingAlongPinnedRay(forwardIt, startSq)){

            // Forward
            if (SquaresToEdge[startSq][forwardIt] > 0){
                targetSq=startSq+SlidingDirections[forwardIt];
                targetPc=Board[targetSq];

                // Check the forward square is empty
                // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                if (Piece::IsFigure(targetPc, Piece::Figure::None) &&(!Check || std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock))){
                    if (nextStepPromote){
                        PromotionMoves(Move(startSq, targetSq), PawnMovesList);
                    } else {
                        PawnMovesList.push_back(Move(startSq, targetSq));
                    };
                };
            };

            // Charging
            // Check if the pawn is in the second row from it's side, if so allow it to charge
            if (startSq >= pawnRowStart && startSq <= pawnRowEnd){
                targetSq=startSq+SlidingDirections[forwardIt]*2;
                targetPc=Board[targetSq];

                // Check the forward square is empty
                // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                if (Piece::IsFigure(targetPc, Piece::Figure::None) && (!Check || std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock))){
                    PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::PawnCharge));
                };
            };

        };

        // Attacking
        for (int i=0; i<2; i++){
            if (SquaresToEdge[startSq][diagonals[diagonalIt][i]] > 0){
                targetSq=startSq+SlidingDirections[diagonals[diagonalIt][i]];
                targetPc=Board[targetSq];

                // Skip attacking direction if pawn is pinned and direction is not in the pinned ray
                if (PinnedRayIt(startSq, PinnedRays) != std::end(PinnedRays) && !IsMovingAlongPinnedRay(diagonals[diagonalIt][i], startSq)){
                    continue;
                };

                // Check that the piece in the diagonal square is an enemy piece
                // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                if (Piece::IsColour(targetPc, ColourEnemy) && (!Check || std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock))){
                    if (nextStepPromote){
                        PromotionMoves(Move(startSq, targetSq), PawnMovesList);
                    } else {
                        PawnMovesList.push_back(Move(startSq, targetSq));
                    };
                };

                // En Passant check
                // Check if the last enemy move was a pawn charge to a square on either side of the current pawn
                // Can't promote on an en passant move so no promotion check
                if (lastMoves[ColourEnemy].flag == Move::MFlag::PawnCharge){

                    // Check for accidential edge wraparound 
                    if (SquaresToEdge[startSq][diagonals[diagonalIt][i]] > 0 && (

                        // If the enemy pawn moved to the east square, check the attacking diagonal is either NW or SW
                        (lastMoves[ColourEnemy].target_square == startSq-1 && (diagonals[diagonalIt][i]==5 || diagonals[diagonalIt][i]==7)) ||
                        // If the enemy pawn moved to the east square, check the attacking diagonal is either NE or SE
                        (lastMoves[ColourEnemy].target_square == startSq+1 && (diagonals[diagonalIt][i]==4 || diagonals[diagonalIt][i]==6))

                    )){
                        // Only add to the move vector if the king won't be in check after the move. Deals with edge case in first video
                        if (!InCheckAfterEP(startSq, targetSq)){
                            PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::EnPassant));
                        };
                    };
                };
            };
        };
    };        

    // Function to calculate the number of squares to generate a list of legal moves at each position
    // Also handles the checkmate and draw checks if there are no legal moves
    std::vector<Move> GenerateMoves(){
        std::vector<Move> MovesList;
        bool inCheck=false, inDoubleCheck=false;
        int startSq;

        // Generate a map of which squares are currently under attack by the enemy
        std::vector<int> SquaresAttacked=SquaresUnderAttack();

        // Generate a 2d vector of pinned squares from the attack information
        std::vector<std::vector<int>> PinnedRays=CalculatePinnedRays(SquaresAttacked);

        // Is the current player in check or double check
        auto check_it=std::find(std::begin(SquaresAttacked), std::end(SquaresAttacked), KingSquares[ColourToMove]);
        if (check_it != std::end(SquaresAttacked)){
            inCheck=true;
            if (std::find(++check_it, std::end(SquaresAttacked), KingSquares[ColourToMove]) != std::end(SquaresAttacked)) inDoubleCheck=true;
        };

        // Iterate over the PieceSquares array instead of the whole board
        for (int sq=0; sq<PieceSquares.size(); sq++){
            startSq=PieceSquares[sq];
            // Check the colour of the piece on the square is the colour to play
            if (Piece::IsColour(Board[startSq], ColourToMove)){
                // Check which type of piece it is
                // King
                if (Piece::IsFigure(Board[startSq], Piece::Figure::King)){
                    GenerateKingMoves(startSq, MovesList, inCheck, SquaresAttacked);
                // Only do other pieces if the player is not in double check
                // Rook, Bishop or Queen
                } else if (Piece::IsSlidingPiece(Board[startSq]) && !inDoubleCheck){
                    GenerateSlidingMoves(startSq, MovesList, inCheck, PinnedRays);
                // Knight
                } else if (Piece::IsFigure(Board[startSq], Piece::Figure::Knight) && !inDoubleCheck){
                    GenerateKnightMoves(startSq, MovesList, inCheck, PinnedRays);
                // Pawn
                } else if (Piece::IsFigure(Board[startSq], Piece::Figure::Pawn) && !inDoubleCheck){
                    GeneratePawnMoves(startSq, MovesList, inCheck, PinnedRays);
                // Empty
                } else {
                    continue;
                };
            };
        };

        // Checkmate and Draws
        if (MovesList.size() == 0){
            if (ColourEnemy==Piece::Colour::White && inCheck){
                std::cout << "Checkmate. White emerges victorious";
            } else if (ColourEnemy==Piece::Colour::Black && inCheck) {
                std::cout << "Checkmate. Black emerges victorious";
            } else{
                std::cout << "Draw, current player has no legal moves but is not in check.";
            };
        };

        return MovesList;
    };

};

// Move notation, if two same pieces can move to the same position, the file is added before the end position. If they are in the same file, the rank is added instead
// Rd8 > Rdd8 or R3d8 for moving a rook

// Functions

// Function to return a unicode charavter based on a square number. Unicode figures for all chess pieces and black and white squares
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


// Function for setting up the board from a FEN string, default argument is the initial board position. Doesn't take the Halfmove clock and Fullmove number information
void SetupBoard(std::string FENstring="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"){
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

    LegalMoves::CastleKingside.assign(2, false), LegalMoves::CastleQueenside.assign(2, false);
    
    auto it2=FENinfo.find(' ', ++it);

    while (it!=it2){
        if (FENinfo[it] == 'K'){
            LegalMoves::CastleKingside[1]=true;
        } else if (FENinfo[it] == 'Q'){
            LegalMoves::CastleQueenside[1]=true;
        } else if (FENinfo[it] == 'k'){
            LegalMoves::CastleKingside[0]=true;
        } else if (FENinfo[it] == 'q'){
            LegalMoves::CastleQueenside[0]=true;
        };

        it++;
    };
    it++;

    // En Passant avaliable
    if (FENinfo[it] !='-'){
        std::string str=FENinfo.substr(it, 2);
        std::cout << LegalMoves::CastleKingside[0] << LegalMoves::CastleKingside[1] << LegalMoves::CastleQueenside[0] << LegalMoves::CastleQueenside[1]<< " " << str << std::endl;
        int initialSq, finalSq=Ref2Idx(str);

        if (ColourEnemy==Piece::Colour::White){
            initialSq=finalSq-16;
        } else {
            initialSq=finalSq+16;
        };

        LegalMoves::lastMoves[ColourEnemy]=Move(initialSq, finalSq, Move::MFlag::PawnCharge);
    };

    // Update KingSquares
    LegalMoves::KingSquares[ColourToMove]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourToMove));});;
    LegalMoves::KingSquares[ColourEnemy]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourEnemy));});;

};

// Function to clear the board
void ClearBoard(){
    for (int i=0; i<Board.size(); i++){
        Piece::Remove(i);
    };
};

// Function to print the board
void Print_Board(int Perspective=ColourToMove){

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


    // Use a switch statement here for the flags of the different moves
    switch (updateMove.flag)
    {
    case Move::MFlag::Castling :
        // Move King
        Piece::Add(updateMove.target_square, ColourToMove, Piece::ReturnFigure(Board[updateMove.initial_square]));
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
        Piece::Remove(updateMove.initial_square);
        break;
    }
    

    // Update the lastMoves array
    LegalMoves::lastMoves[ColourToMove]=updateMove;


    // Castling checks after possible castling moves

    // Updates anytime the king moves in the 5th file
    if (Piece::IsFigure(Board[updateMove.initial_square], Piece::King)){
        if (updateMove.initial_square%8 == 4){
            LegalMoves::CastleQueenside[ColourToMove]=false;
            LegalMoves::CastleKingside[ColourToMove]=false;
        };

    // Updates anytime a rook moves in the edge files
    } else if (Piece::IsFigure(Board[updateMove.initial_square], Piece::Rook)){
        // Left file
        if (updateMove.initial_square%8 == 0){
            LegalMoves::CastleQueenside[ColourToMove]=false;
        // Right file
        } else if (updateMove.initial_square%8 == 7){
            LegalMoves::CastleKingside[ColourToMove]=false;
        };
    };

    // Update KingSquare array if the initial square was the king square
    if (updateMove.initial_square == LegalMoves::KingSquares[ColourToMove]) LegalMoves::KingSquares[ColourToMove]=*std::find_if(std::begin(PieceSquares), std::end(PieceSquares), [](const int& i){return (Piece::IsFigure(Board[i], Piece::Figure::King) && Piece::IsColour(Board[i], ColourToMove));});;


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