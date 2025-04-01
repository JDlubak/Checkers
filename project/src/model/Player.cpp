#include <algorithm>
#include <sstream>
#include <utility>
#include "Player.h"
#include "Piece.h"
#include "UI.h"
#include "State.h"
/// konstruktor ustawiający wszystkie pola
Player::Player(std::string name, bool playedColor, const std::vector<PiecePtr> &piecesLeft) : name(std::move(name)),
                                                                                               playedColor(playedColor),
                                                                                               piecesLeft(piecesLeft) {}
/// getter nazwy
const std::string &Player::getName() const {
    return name;
}
/// setter nazwy
void Player::setName(const std::string &newName) {
    Player::name = newName;
}
/// getter koloru gracza
bool Player::getPlayedColor() const {
    return playedColor;
}
/// setter koloru gracza
void Player::setPlayedColor(bool newPlayedColor) {
    Player::playedColor = newPlayedColor;
}
/// wektor figur gracza
std::vector<PiecePtr> &Player::getPiecesLeft() {
    return piecesLeft;
}
/// setter figur gracza
void Player::setPiecesLeft(const std::vector<PiecePtr> &newPiecesLeft) {
    Player::piecesLeft = newPiecesLeft;
}
/// destruktor
Player::~Player() = default;
/// getter informacji o tym, czy gracz jest człowiekiem
bool Player::getIsHuman() const {
    return isHuman;
}
/// setter informacji o tym, czy gracz jest człowiekiem
void Player::setIsHuman(bool newIsHuman) {
    Player::isHuman = newIsHuman;
}
/*
 * Szukamy elementu w wektorze piecesLeft, po x i po y
 * Jeśli znajdziemy figurę, to ją zwracamy, w przeciwnik razie, zwracamy nullptr
 */
PiecePtr Player::findPiece(const std::vector<PiecePtr>& piecesLeft, int x, int y) {
    auto it = std::find_if(piecesLeft.begin(), piecesLeft.end(),
            // Porownanie x i y figury ze wspolrzednymi x i y przekazanymi do funkcji
                           [x, y](const PiecePtr& piece) {
                               return piece->getX() == x && piece->getY() == y;
                           });
    // Jesli znaleziono element, czyli iterator it nie jest na koncu wektora piecesLeft, to go zwroc
    if (it != piecesLeft.end()) return *it;
    // Jesli nie znaleziono elementu, zwroc nullptr
    return nullptr;
}
/*
 * Metoda przyjmuje wskaźnik do aktualnego stanu, oraz informację o tym, co sprawdzamy - wybór figury czy wybór docelowego ruchu
 * Jeśli dana figura/dane pole może zostać wybrane, zwracamy prawdę, w przeciwnym razie zwracamy fałsz i wymuszamy ponowny wybór
 */
bool Player::moveVerification(const StatePtr& stan, bool whichPartNeedsToBeVerified) {
    if (whichPartNeedsToBeVerified) {           // jesli prawda - sprawdzmay wybor figury
        // switch (od czegos tam, do ktorego ciezko sie dostac) - od informacji w tablicy currentPosition dla indeksu pola wybranej figury
        // czyli po prostu sprawdzamy, czy na polu wybranym stoi pionek, krolowa danego koloru lub czy pole jest puste
        switch (stan->getCurrentPosition()[stan->convertStringToArrayIndex(stan->getSelectedPieceString())]) {
            case 0:         // pole jest puste
                UI::badOutput(1);       // komunikat bledu
                return false;           // tej figury nie mozna wybrac
            case 1:     // bialy pionek
            case 2:     // biala krolowa
                if (getPlayedColor()) { // jesli gra czarny
                    UI::badOutput(2); // komunikat bledu
                    return false;       // tej figury nie mozna wybrac
                }
                break;
            case 3:     // czarny pionek
            case 4:     // czarna krolowa
                if (!getPlayedColor()) { // jesli nie gra czarny
                    UI::badOutput(2);  // komunikat bledu
                    return false;        // tej figury nie mozna wybrac
                }
                break;
        }
        /*
         * W tym miejscu moze trzy uwagi
         * 1 - komputer nie moze wybrac figur przeciwnika lub pustego pola, bo losuje figure ze swojego wektora, więc komunikaty błędu nie będą wypisane, a figura zostanie ustawiona zawsze
         * 2 - jesli wpisano zle pole - funkcja zwróciła juz falsz
         * 3 - jesli wpisano dobre pole - figura zostanie odnaleziona i ustawiona w stanie
         * Wniosek - zawsze figura bedzie wybrana dla dalszej czesci
         */
        if (!stan->getSelectedPiece()->checkIfThisPieceCanMove(stan)) {     // jesli wybrana figura nie moze sie ruszac
            if (getIsHuman()) UI::badOutput(3);           // jesli uzytkownik wybral zla figure, wyswietl komunikat bledu
            return false;                        // ta figura nie moze zostac wybrana
        } else if (!stan->getSelectedPiece()->checkIfThisPieceIsAbleToCapture()) {      // jesli figura moze sie ruszac, sprawdz, czy nie moze bic
            for (const PiecePtr& p: getPiecesLeft()) {       // jesli nie moze, rozpatrz swoj caly wektor pod katem tego, czy dowolna figura moze zbijac
                if (p->checkIfThisPieceIsAbleToCapture()) {        // jesli inna figura moze zbijac
                    if (getIsHuman()) UI::badOutput(4);      // jesli uzytkownik wybral zla figure, wyswietl komunikat bledu
                    return false;  // ta figura nie moze zostac wybrana
                }
            }
        }
    } else {
        if (!stan->getSelectedPiece()->checkIfThisMoveIsPossible(stan)) {       // sprawdzamy tylko, czy ruch jest mozliwy na docelowe pole - zadne inne warunki tu sa niepotrebne
            if (getIsHuman()) UI::badOutput(5);       // jesli uzytkownik wpisal zle pole, wyswietl komunikat bledu
            return false;           // ten ruch jest nielegalny
        }
    }
    return true; // prawda zostanie zwrocona, jesli funkcja skutecznie przejdzie przez wszystkie warunki - figure mozna wybrac/ruch jest legalny
}
/// łączymy w całość wszystkie informacje o graczu i je zwracamy w postaci napisu
std::string Player::getPlayerInfo() const {
    std::ostringstream ss;
    ss << (getPlayedColor() ? "Czarny" : "Biały")           // w zaleznosci od granego koloru,  ustaw odpowiedni napis
       << (getIsHuman() ? " Gracz: " : " BOT: ")              // w zaleznosci od faktu, czy gracz jest czlowiekiem, czy komputeren, ustaw odpowiedni napis
       << getName();                    // dodaj nazwe gracza
    return ss.str();
}


