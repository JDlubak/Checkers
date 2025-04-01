#ifndef COMPUTER_H
#define COMPUTER_H

#include "Player.h"

class Computer : public Player {
public:
    Computer(const std::string &name, bool playedColor, const std::vector<PiecePtr> &piecesLeft);

    void makeMove(StatePtr stan) override;

    ~Computer() override;
};

#endif 
