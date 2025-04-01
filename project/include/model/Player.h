#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string>
#include "typedefs.h"
class Player : public std::enable_shared_from_this<Player> {
private:
    std::string name;
    bool playedColor;
    std::vector<PiecePtr> piecesLeft;
    bool isHuman;
public:
    Player(std::string name, bool playedColor, const std::vector<PiecePtr> &piecesLeft);

    const std::string &getName() const;

    void setName(const std::string &name);

    bool getPlayedColor() const;

    void setPlayedColor(bool playedColor);

    bool getIsHuman() const;

    void setIsHuman(bool isHuman);

    std::vector<PiecePtr> &getPiecesLeft();

    void setPiecesLeft(const std::vector<PiecePtr> &piecesLeft);

    virtual ~Player() = 0;

    virtual void makeMove(StatePtr stan) = 0;

    static PiecePtr findPiece(const std::vector<PiecePtr> &piecesLeft, int x, int y);

    bool moveVerification(const StatePtr& stan, bool whichPartNeedsToBeVerified);

    virtual std::string getPlayerInfo() const;

};

#endif 
