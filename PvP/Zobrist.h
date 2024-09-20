#ifndef ZOBRIST_H
#define ZOBRIST_H


// Zobrist Hashing section, to do with remembering all of the previous board states
// Need 64 bit random numbers, so unsigned long long int

// Random numbers of each type of piece of each colour on each square of the board, initialised with sizes 6, 2, 64
std::vector<std::vector<std::vector<unsigned long long>>> pieceRndNums(6, std::vector<std::vector<unsigned long long>>(2, std::vector<unsigned long long>(64)));

// Random numbers for if each side has castling rights
std::vector<unsigned long long> castlingRndNums(16);

// Random numbers to indicate a valid en-passant file
std::vector<unsigned long long> enpassantRndNums(8);

// Final random number, needed for the side to move. Only hashed with the key if it is black's turn to move
unsigned long long sideToMoveRndNum;


namespace Zobrist{

    // Function to compute the 64 bit random numbers needed to perform Zobrist hashing
    void ZobristInit();

    // Function to calculate the Zobrist key from a given position. As the advantage of using zobrist hashing is the speed that the key can be updated incrementally, this should only
    // be used when setting up the board
    unsigned long long CalculateZobristKey(std::vector<int> Board, std::vector<int> PieceSquares);


};

#endif