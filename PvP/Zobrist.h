#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <vector>

// Zobrist Hashing section, to do with remembering all of the previous board states
// Need 64 bit random numbers, so unsigned long long int

namespace Zobrist{

    // Function to compute the 64 bit random numbers needed to perform Zobrist hashing
    void ZobristInit();

    // Function to convert the vector of boolean castling rights into a 4 digit binary number
    int CastlingIndex(std::vector<std::vector<bool>>& CastlingRights);

    // Function to calculate the Zobrist key from a given position. As the advantage of using zobrist hashing is the speed that the key can be updated incrementally, this should only
    // be used when setting up the board
    unsigned long long CalculateZobristKey(std::vector<int> Board, std::vector<int> PieceSquares);

};

#endif