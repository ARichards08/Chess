#include "Move.h"

// Default constructor
Move::Move(int initial/*=0*/, int target/*=0*/, MFlag f/*=None*/){
    initial_square=initial;
    target_square=target;
    flag=f;
};

// Copy constructor
Move::Move(const Move &in_move){
    initial_square=in_move.initial_square;
    target_square=in_move.target_square;
    flag=in_move.flag;
};

// Constructor for a move and a new flag
Move::Move(const Move &in_move, MFlag f){
    initial_square=in_move.initial_square;
    target_square=in_move.target_square;
    flag=f;
};