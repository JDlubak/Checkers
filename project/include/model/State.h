#ifndef STATE_H
#define STATE_H
#include <string>
#include "typedefs.h"
class UI;
class State : public std::enable_shared_from_this<State> {
private:
    bool currentColor;
    int currentPosition[64];
    /*
     * 0 - puste pole
     * 1 - bialy pionek
     * 2 - biala krolowa
     * 3 - czarny pionek
     * 4 - czarna krolowa
     * Liczone od lewego dolnego rogu, co 8 przejscie do rzedu powyzej
     */
    PlayerPtr white;
    PlayerPtr black;
    std::string playedMove;
    std::string selectedPieceString;
    PiecePtr selectedPiece;
public:
    State();
    State(bool currentColor, const int *currentPosition, PlayerPtr white, PlayerPtr black);

    bool getCurrentColor() const;

    void setCurrentColor(bool currentColor);

    const int *getCurrentPosition() const;

    void setCurrentPosition(const int *currentPosition);

    int getPieceInformation(int index) const;

    void changePieceInformation(int index, int newInformation);

    PlayerPtr getWhite() const;

    void setWhite(PlayerPtr white);

    PlayerPtr getBlack() const;

    void setBlack(PlayerPtr black);

    const std::string &getPlayedMove() const;

    void setPlayedMove(const std::string &playedMove);

    const std::string &getSelectedPieceString() const;

    void setSelectedPieceString(const std::string &selectedPieceString);

    PiecePtr getSelectedPiece() const;

    void setSelectedPiece(PiecePtr selectedPiece);

    void prepareInitialState(bool isLoadedFromFile);

    static int convertStringToArrayIndex(std::string testString) ;

    void playGame();

    bool decideIfGameShouldContinue();

    virtual ~State();

};

#endif 
