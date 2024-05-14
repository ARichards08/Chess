#ifndef MOVE_H
#define MOVE_H

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
    Move(int initial=0, int target=0, MFlag f=None);

    // Copy constructor
    Move(const Move &in_move);

    // Constructor for a move and a new flag
    Move(const Move &in_move, MFlag f);
};

#endif