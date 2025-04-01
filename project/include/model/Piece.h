#ifndef PIECE_H
#define PIECE_H
#include <vector>
#include "typedefs.h"

class Piece : public std::enable_shared_from_this<Piece> {
private:
    int x;                      // 1 - A, 2 - B etc.; 8 - H
    int y;                      // 1-8
    bool color;                 // 0 - white piece, 1 - black Piece
    bool isPromoted;            // 0 - Pawn, 1 - Queen
    bool isAbleToCapture;       // dodalem dodatkowe pole, bo nie wiem jak inaczej wymusic na graczu bicie
public:
    Piece(int x, int y, bool color);

    int getX() const;

    void setX(int newX);

    int getY() const;

    void setY(int y);

    bool getColor() const;

    void setColor(bool color);

    bool getPromotion() const;

    void setPromotion(bool isPromoted);

    bool checkIfThisPieceIsAbleToCapture() const;

    void setIsAbleToCapture(bool isAbleToCapture);

    virtual ~Piece() = 0;

    void killOpponentPiece(const StatePtr& stan) const;

    void promoteIfOnBackRank(const StatePtr& stan);

    void move(const StatePtr& stan);

    virtual bool checkIfThisMoveIsPossible(StatePtr stan, int test=-1) = 0;

    bool checkIfThisPieceCanMove(const StatePtr& stan);
};

#endif

