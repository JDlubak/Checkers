#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece {
public:
    Pawn(int x, int y, bool color);

    ~Pawn() override;

    bool checkIfThisMoveIsPossible(StatePtr stan, int test=-1) override;

    bool forwardMoveCheck(int roznica, int curPos, int pole, const StatePtr& stan);

    bool captureMoveCheck(int roznica, int curPos, int pole, const StatePtr& stan);
};

#endif 
