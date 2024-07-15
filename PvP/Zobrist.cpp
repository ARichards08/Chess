#include "Zobrist.h"

#include <vector>
#include <random>

#include "pcg_random.hpp"
#include "Piece.h"
#include "Chess2P.h"


// Zobrist Hashing section, to do with remembering all of the previous board states
// Need 64 bit random numbers, so unsigned long long int

// Fucntion to compute the 64 bit random numbers needed to perform Zobrist hashing
void ZobristInit(){

    

    // PRNG setup
    
    // Only if wanting to seed with a random number
    //pcg_extras::seed_seq_from<std::random_device> seed_source;
        
    pcg64 PRNG_engine(206005392);

    std::uniform_int_distribution<unsigned long long> uni_dist(0, UINT64_MAX);

    // Piece random numbers
    for (int i=0; i<6; i++){
        for (int j=0; j<2; j++){
            for (int k=0; k<64; k++){
                pieceRndNums[i][j][k]=uni_dist(PRNG_engine);
            };
        };
    };

    // Castling rights random numbers
    for (int i=0; i<16; i++){
        castlingRndNums[i]=uni_dist(PRNG_engine);
    };

    // Side to move random number
    sideToMoveRndNum=uni_dist(PRNG_engine);

};

// Function to calculate the Zobrist key from a given position. As the advantage of using zobrist hashing is the speed that the key can be updated incrementally, this should only
// be used when setting up the board
unsigned long long CalculateZobristKey(std::vector<int> Board, std::vector<int> PieceSquares){

    unsigned long long zobristKey=0;

    // First index, find the type of piece, cast to int and -1, as we do not care about None pieces
    // Second index, checks the colour of the piece and if it is black returns 0 otherwise 1 for the colour index
    // Third index, just the PieceSquares entry

    // Updates the zobristKey by XORing each piece's hash with the key
    for (int i=0; i<PieceSquares.size(); i++){
        int piecefigure=(int) Piece::ReturnFigure(Board[PieceSquares[i]])-1;
        int piececolour=(Piece::IsColour(Board[PieceSquares[i]], Piece::Colour::Black)) ? 0 : 1;
        
        zobristKey^=pieceRndNums[piecefigure][piececolour][PieceSquares[i]];
    };

    // Updates the zobristKey with ep file

    // Updates the zobristKey with castling rights


    // Updates the zobristKey with colour to move
    if (ColourToMove==0) zobristKey^=sideToMoveRndNum;

    return zobristKey;

};