#include "Human.h"
#include "State.h"
#include "Piece.h"
#include "UI.h"
/// konstruktor - ustawiamy setHuman true
Human::Human(const std::string &name, bool playedColor, const std::vector<PiecePtr> &piecesLeft,
             const pt::time_duration &remainingTime) : Player(name, playedColor, piecesLeft),
                                                       remainingTime(remainingTime) {
    setIsHuman(true);
}
/// destruktor
Human::~Human() = default;
/// Metoda otrzymuje wskaźnik do aktualnego stanu gry. Gracz wykonuje ruch w 2 fazach. Najpierw wybiera pole, na którym znajduje się jego figura.
/// Pole musi zostać wybrane poprawnie - musi zawierać figurę gracza, która musi mieć możliwość poruszania, a w przypadku, gdy ta figura nie może zbijać, to to pole można wybrać tylko w sytuacji, gdy żadna inna figura gracza nie może zbijać.
/// Następnie, przechodzimy do 2 fazy - wyboru pola docelowego. Ruch musi być legalny wedle reguł warcabów, a jeśli figura ma bicie, to musi zbijać.
/// Po wpisaniu prawidłowego ruchu zostaje on wykonany - wywoływana jest metoda move dla figury, w celu poruszenia się
/// W przypadku, gdy dana figura może zbijać, wywoływana jest metoda move tyle razy, ile jest możliwych bić, i po każdym ruchu następuje zbicie figury poprzez wywołanie metody killOpponentPiece
void Human::makeMove(StatePtr stan) {
    bool loopController = true;
    while (true) {
        // po prostu ustawiamy wybrana figure poprzez wpisanie pola, na ktorym sie znajduje
        // isTarget: 0 - informacja dla metody, ze chodzi o pole figury
        // parametr !loopController za 1 przejsciem da falsz - wyswietl pierwsza linie komunikatu, a za ewentualnymi kolejnymi bedzie prawda, czyli ja ominie
        stan->setSelectedPieceString(UI::selectSquare(false, !loopController));
        int y = (stan->convertStringToArrayIndex(stan->getSelectedPieceString()) + 8) / 8;  // y figury uzyskany przez odpowiednia konwersje z indeksu wybranego pola
        int x = (stan->convertStringToArrayIndex(stan->getSelectedPieceString()) % 8) + 1;  // x figury uzyskany przez odpowiednia konwersje z indeksu wybranego pola
        // szukaj w wektorze figury czarnego po x i y, jesli gra czarny
        if (getPlayedColor()) stan->setSelectedPiece(stan->getBlack()->findPiece(stan->getBlack()->getPiecesLeft(), x, y));
        // w przeciwnym razie szukaj w wektorze bialego po x i y
        else stan->setSelectedPiece(stan->getWhite()->findPiece(stan->getWhite()->getPiecesLeft(), x, y));
        // zmien wartosc loopControllera w zaleznosci od tego, czy wpisano dobre pole figury
        // isUserGenerated: 1 - wyswietl komunikaty bledu
        // whichPartNeedsToBeVerified: 1 - metoda weryfikuje obydwie fazy ruchu, zatem informujemy ja, ze chodzi o wybor figury
        loopController = moveVerification(stan, true);
        if (loopController) break;  // jesli wpisane pole przeszlo test metody, przerwij petle i przejdz do wyboru pola docelowego
    }
    while (true) {
        // po prostu ustawiamy docelowe pole poprzez wpisanie go
        // isTarget: 1 - informacja dla metody, ze chodzi o pole docelowe
        // parametr !loopController za 1 przejsciem da falsz - wyswietl pierwsza linie komunikatu, a za ewentualnymi kolejnymi bedzie prawda, czyli ja ominie
        stan->setPlayedMove(UI::selectSquare(true, !loopController));
        // zmien wartosc loopControllera w zaleznosci od tego, czy wpisano dobre pole figury
        // isUserGenerated: 1 - wyswietl komunikaty bledu
        // whichPartNeedsToBeVerified: 0 - metoda weryfikuje obydwie fazy ruchu, zatem informujemy ja, ze chodzi o wybor pola docelowego
        loopController = moveVerification(stan, false);
        if (loopController) {   // jesli wpisane pole przeszlo test metody, rozpatrz kolejny warunek ponizej
            if (stan->getSelectedPiece()->checkIfThisPieceIsAbleToCapture()) {
                stan->getSelectedPiece()->move(stan);
                stan->getSelectedPiece()->killOpponentPiece(stan); // Jesli figura mogla zbijac, to ja rusz, a nastepnie zbij figure przeciwnika
                stan->getSelectedPiece()->setIsAbleToCapture(false);  // zaloz ze figura nie moze dalej zbijac, po czym sprawdz, czy masz racje poprzez petle wywolujaca metode checkIfThisMoveIsPossible
                for (int i = 0; i < 64; i++) stan->getSelectedPiece()->checkIfThisMoveIsPossible(stan, i);
                stan->setSelectedPieceString(stan->getPlayedMove());  // Przestaw indeks aktualnie wybranej figury na indeks pola docelowego
            } else stan->getSelectedPiece()->move(stan); // jesli figura nie moze zbijac, to tylko ja rusz
        }
        if (loopController && !stan->getSelectedPiece()->checkIfThisPieceIsAbleToCapture()) break;
        /*
         * W tym miejscu wazna uwaga
         * Mamy 2 warunki zakonczenia petli - wpisane pole przeszlo pole metody oraz figura juz nie moze zbijac
         * Zakladajac poprawne wpisane docelowe, mamy 2 mozliwosci:
         * a) figura nie moze zbijac i b) figura moze zbijac, rozpatrujac je mamy:
         * a) po ruchu figura "dalej nie moze zbijac" - bo program nie wywoluje metody weryfikujacej to
         * b) po ruchu jest sprawdzany warunek, czy figura moze dalej zbijac: jesli moze, to program przejdzie petle jeszcze raz, a jesli nie, to petla sie zakonczy
         * Dlatego nie ma szans na sytuacje, gdzie np. w pierwszym ruchu ruszamy sie pionkiem o 1 pole, ale nagle moze zbijac (z punktu widzenia czlowieka) i zaczynamy zbijanie
         */
    }
}
/// getter pozostałego czasu człowieka
const pt::time_duration &Human::getRemainingTime() const {
    return remainingTime;
}
/// setter pozostałego czasu człowieka. Jeśli czas wyjdzie ujemny, ustawiamy go na 0 sekund
void Human::setRemainingTime(const pt::time_duration &newRemainingTime) {
    remainingTime = newRemainingTime;
    if (getRemainingTime()<pt::seconds(0)) remainingTime=pt::seconds(0);
}
/// getter informacji o graczu, wykorzystujący metodę getPlayerInfo i poszerzający informacje z niej o pozostały czas
std::string Human::getPlayerInfo() const {
    std::ostringstream ss;
    ss << Player::getPlayerInfo();
    ss << ", pozostały czas: " << getRemainingTime();
    return ss.str();
}






