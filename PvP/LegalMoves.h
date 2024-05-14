#ifndef LEGALMOVES_H
#define LEGALMOVES_H

#include <vector>
#include "Move.h"

// Namespace with all the functions related to validating a legal move
namespace LegalMoves{

    extern const std::vector<int> SlidingDirections;

    extern const std::vector<std::vector<int>> diagonals;

    extern std::vector<bool> CastleQueenside, CastleKingside;

    extern std::vector<int> SquaresToBlock;

    // Vector to hold the squares the king occupys
    extern std::vector<int> KingSquares;

    // Static vector that contains the number of squares in 8 directions
    // Precomputing function will be called to set this up.
    std::vector<std::vector<int>> PrecomputingData();

    const static std::vector<std::vector<int>> SquaresToEdge(PrecomputingData());

    void GenerateSlidingAttacks(int startSq, std::vector<int>& SquaresAttacked);

    void GenerateKnightAttacks(int startSq, std::vector<int>& SquaresAttacked);

    void GeneratePawnAttacks(int startSq, std::vector<int>& SquaresAttacked);

    void GenerateKingAttacks(int startSq, std::vector<int>& SquaresAttacked);

    std::vector<int> SquaresUnderAttack();

    std::vector<std::vector<int>> CalculatePinnedRays(const std::vector<int>& SquaresAttacked);

    std::vector<std::vector<int>>::iterator PinnedRayIt(const int& sq, std::vector<std::vector<int>>& PinnedRays);

    bool IsMovingAlongPinnedRay(const int& dir, const int& startSq);

    void GenerateSlidingMoves(int startSq, std::vector<Move>& SlidingMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays);

    void GenerateKnightMoves(int startSq, std::vector<Move>& KnightMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays);

    void GenerateKingMoves(int startSq, std::vector<Move>& KingMovesList, const bool& inCheck, const std::vector<int>& SquaresAttacked);

    void PromotionMoves(Move PawnMove, std::vector<Move>& List);

    bool InCheckAfterEP(const int & startSq, const int& epTargetSq);

    void GeneratePawnMoves(int startSq, std::vector<Move>& PawnMovesList, const bool& Check, std::vector<std::vector<int>>& PinnedRays);        

    std::vector<Move> GenerateMoves();

};

#endif