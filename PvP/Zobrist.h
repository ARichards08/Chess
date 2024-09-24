#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <vector>
#include "Piece.h"

// Zobrist Hashing section, to do with remembering all of the previous board states
// Need 64 bit random numbers, so unsigned long long int

namespace Zobrist{

    // Variable to hold the current zobristKey, 
    extern unsigned long long key;

    // Function to compute the 64 bit random numbers needed to perform Zobrist hashing
    void ZobristInit();

    // Function to convert the vector of boolean castling rights into a 4 digit binary number
    int CastlingIndex(std::vector<std::vector<bool>>& CastlingRights);

    // Function to calculate the Zobrist key from a given position. As the advantage of using zobrist hashing is the speed that the key can be updated incrementally, this should only
    // be used when setting up the board
    unsigned long long CalculateZobristKey(std::vector<int> Board, std::vector<int> PieceSquares);

    // Function to Add or Remove a piece on a square from the zobrist key
    void UpdatePieceZobristKey(int square, int pcol, Piece::Figure pfig);

    // Function to update castling rights in the zobrist key
    void UpdateCastlingZobristKey(std::vector<std::vector<bool>> oldCastlingRights, std::vector<std::vector<bool>> CastlingRights);

    // Function to update enpassant file in the zobrist key
    void UpdateEnPassantZobristKey(int oldEPFile, int EPFile);

    // Function to update the side to move in the zobrist key
    void UpdateSideToMoveZobristKey();

};

#endif