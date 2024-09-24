#include "Zobrist.h"

#include <vector>
#include <random>

#include "pcg_random.hpp"
#include "Piece.h"
#include "Chess2P.h"


namespace Zobrist{


    // Zobrist Hashing section, to do with remembering all of the previous board states
    // Need 64 bit random numbers, so unsigned long long int

    // Key definition
    unsigned long long key=0;

    // Random numbers of each type of piece of each colour on each square of the board, initialised with sizes 6, 2, 64
    std::vector<std::vector<std::vector<unsigned long long>>> pieceRndNums(6, std::vector<std::vector<unsigned long long>>(2, std::vector<unsigned long long>(64)));

    // Random numbers for if each side has castling rights
    std::vector<unsigned long long> castlingRndNums(16);

    // Random numbers to indicate a valid en-passant file
    std::vector<unsigned long long> enpassantRndNums(8);

    // Final random number, needed for the side to move. Only hashed with the key if it is black's turn to move
    unsigned long long sideToMoveRndNum;

    // Function to compute the 64 bit random numbers needed to perform Zobrist hashing
    void ZobristInit(){
        // PRNG setup
        
        // Only if wanting to seed with a random number
        // pcg_extras::seed_seq_from<std::random_device> seed_source;
        // 206005392 is a randomly chosen number, at some point it might be worth using a number that is found to have less clashes/collisions
            
        pcg64 PRNG_engine(206005392);

        std::uniform_int_distribution<unsigned long long> uni_dist(0, UINT64_MAX);

        // Piece random numbers
        // i = piece type, j = piece colour, k = piece square
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

        // En Passant File random numbers
        for (int i=0; i<8; i++){
            enpassantRndNums[i]=uni_dist(PRNG_engine);
        };

        // Side to move random number
        sideToMoveRndNum=uni_dist(PRNG_engine);
    };


    // Function to convert the vector of booleans related to the castling rights into an binary index for the Zobrist random numbers for castling
    // Order of caslting directions for binary positions:
    // 2^3, 2^2, 2^1, 2^0
    // WK,  WQ,  BK,  BQ
    int CastlingIndex(std::vector<std::vector<bool>>& CastlingRights){
        std::string index="";

        for (int i=0; i<2; i++){
            for (int j=0; j<2; j++){
                index+=CastlingRights[i][j] ? "1" : "0";
            };
        };

        return std::stoi(index, nullptr, 2); // nullptr so that we can specify the third arguement, the base of the integer
    };

    // Function to calculate the Zobrist key from a given position. As the advantage of using zobrist hashing is the speed that the key can be updated incrementally, this should only
    // be used when setting up the board
    unsigned long long CalculateZobristKey(std::vector<int> Board, std::vector<int> PieceSquares){
        key=0;

        // First index, find the type of piece, cast to int and -1, as we do not care about None pieces
        // Second index, checks the colour of the piece and if it is black returns 0 otherwise 1 for the colour index
        // Third index, just the PieceSquares entry

        // Updates the key by XORing each piece's hash with the key
        for (int i=0; i<PieceSquares.size(); i++){
            int piecefigure=(int) Piece::ReturnFigure(Board[PieceSquares[i]])-1;
            int piececolour=(Piece::IsColour(Board[PieceSquares[i]], Piece::Colour::Black)) ? 0 : 1;
            
            key^=pieceRndNums[piecefigure][piececolour][PieceSquares[i]];
        };

        // Updates the key with ep file
        if (EnPassantFile != -1) key^=enpassantRndNums[EnPassantFile];
        
        // Updates the key with castling rights
        key^=castlingRndNums[CastlingIndex(CastlingRights)];

        // Updates the key with colour to move
        if (ColourToMove==0) key^=sideToMoveRndNum;

        return key;
    };

    // Function to add or remove a piece on a square from the zobrist key
    void UpdatePieceZobristKey(int square, int pcol, Piece::Figure pfig){
        key^=pieceRndNums[pfig][pcol][square];
    };

    // Function to update castling rights in the zobrist key
    void UpdateCastlingZobristKey(std::vector<std::vector<bool>> oldCastlingRights, std::vector<std::vector<bool>> CastlingRights){
        key^=castlingRndNums[CastlingIndex(oldCastlingRights)];
        key^=castlingRndNums[CastlingIndex(CastlingRights)];
    };

    // Function to update enpassant file in the zobrist key
    void UpdateEnPassantZobristKey(int oldEPFile, int EPFile){
        if (oldEPFile != -1)key^=enpassantRndNums[oldEPFile];
        if (EPFile != -1) key^=enpassantRndNums[EPFile];
    };

    // Function to update the side to move in the zobrist key
    void UpdateSideToMoveZobristKey(){
        key^=sideToMoveRndNum;
    };

};