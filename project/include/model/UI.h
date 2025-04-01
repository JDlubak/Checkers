#ifndef UI_H
#define UI_H
#include <string>
#include "typedefs.h"
class State;

class UI {
public:
    static void printCurrentPosition(const StatePtr& stan, bool theEnd);

    static std::string selectSquare(bool isTarget, bool skipFirstLine);

    static void badOutput(int x);

    static void setPlayerInformationUsingUserInput(const StatePtr& stan);

    static bool saveGame(const StatePtr& stan, bool notTesting, std::string saveFileName = std::string());

    static bool loadGame(const StatePtr& stan, bool notTesting, std::string saveFileName = std::string());

    static void mainMenu();

    static void printGameRules();

    static void selectGameTime(const StatePtr& stan);
};

#endif 
