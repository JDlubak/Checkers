#ifndef HUMAN_H
#define HUMAN_H

#include "Player.h"
class Human : public Player {
private:
    pt::time_duration remainingTime;
public:
    Human(const std::string &name, bool playedColor, const std::vector<PiecePtr> &piecesLeft,
          const pt::time_duration &remainingTime);

    void makeMove(StatePtr stan) override;

    const pt::time_duration &getRemainingTime() const;

    void setRemainingTime(const pt::time_duration &remainingTime);

    ~Human() override;

    std::string getPlayerInfo() const override;

};



#endif 
