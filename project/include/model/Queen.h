#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"
class State;

class Queen : public Piece {
public:
    Queen(int x, int y, bool color);

    bool checkIfThisMoveIsPossible(StatePtr stan, int test=-1) override;

    ~Queen() override;
};


#endif 
