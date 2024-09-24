#include "LegalMoves.h"

#include <vector>
#include <iostream>
#include <algorithm>

#include "Chess2P.h"
#include "Piece.h"
#include "BoardFunctions.h"

namespace LegalMoves{

    // Direction offset constants to move sliding pieces around the board
    // const vectors aren't allowed, arrays only it seems
    // North, South, East, West, NE, SW, SE, NW
    const std::vector<int> SlidingDirections={8, -8, 1, -1, 9, -9, -7, 7};

    // Setup Knight diagonals for each cardinal direction
    // North, South, East, West
    const std::vector<std::vector<int>> diagonals={{4, 7}, {5, 6}, {4, 6}, {5, 7}};

    // Vector of squares that can be moved to to block a check
    std::vector<int> SquaresToBlock;

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
            if (CastlingRights[ColourToMove][CastlingSide::Queenside]==true){
                // If the three squares between the left Rook and the King are empty, and the Rook square does have a rook, add the move
                if (Piece::IsFigure(Board[startSq-1], Piece::Figure::None) && Piece::IsFigure(Board[startSq-2], Piece::Figure::None) && Piece::IsFigure(Board[startSq-3], Piece::Figure::None) && Piece::IsFigure(Board[startSq-4], Piece::Figure::Rook)){
                    // Add King move if the target square is not under attack
                    if (std::find(std::begin(SquaresAttacked), std::end(SquaresAttacked), startSq-3) == std::end(SquaresAttacked)) KingMovesList.push_back(Move(startSq, startSq-3, Move::MFlag::Castling));
                };
            };
            // Same again but for right
            if (CastlingRights[ColourToMove][CastlingSide::Kingside]==true){
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
        int targetSq, targetPc, forwardIt, diagonalIt, pawnRow, EPRow;

        // Promote if pawn is in either in the tope or bottom row
        bool nextStepPromote = startSq>55 || startSq<8;

        // Only move N, NE or NW when the player colour is white
        if (ColourToMove==Piece::Colour::White){
            forwardIt=0;
            diagonalIt=0;
            pawnRow=1;
            EPRow=5;
        // Only move S, SE or SW when the player colour is black
        } else {
            forwardIt=1;
            diagonalIt=1;
            pawnRow=6;
            EPRow=2;
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

                // Charging
                // Check if the pawn is in the second row from it's side, if so allow it to charge
                if (Row(startSq)==pawnRow){

                    // Have to check that the first square forwards is empty as well
                    // targetPc is still targeting the square one in front of the pawn
                    if (Piece::IsFigure(targetPc, Piece::Figure::None)){

                        targetSq=startSq+SlidingDirections[forwardIt]*2;
                        targetPc=Board[targetSq];

                        // Check the forward square is empty
                        // Only add to the vector if the the king isn't in check, or if the target square is in SquaresToBlock, to block a check
                        if (Piece::IsFigure(targetPc, Piece::Figure::None) && (!Check || std::find(std::begin(SquaresToBlock), std::end(SquaresToBlock), targetSq)!=std::end(SquaresToBlock))){
                            PawnMovesList.push_back(Move(startSq, targetSq, Move::MFlag::PawnCharge));
                        };
                    };
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

                // En Passant check
                // Check if the valid En Passant file is the same file that the pawn wants to attack, and then check if the pawn is moving into the correct row for it to be an EP capture
                if (File(targetSq)==EnPassantFile && Row(targetSq)==EPRow){
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
                std::cout << "Checkmate. White emerges victorious.";
            } else if (ColourEnemy==Piece::Colour::Black && inCheck) {
                std::cout << "Checkmate. Black emerges victorious.";
            } else{
                std::cout << "Draw, current player has no legal moves but is not in check.";
            };
        };

        return MovesList;
    };


};