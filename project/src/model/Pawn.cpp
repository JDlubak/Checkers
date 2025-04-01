#include "Pawn.h"
#include "State.h"
#include <cmath>
/// Konstruktor dla pionka. Ustawia on pole setPromotion na false - bo pionek nie po promocji (jeszcze)
Pawn::Pawn(int x, int y, bool color) : Piece(x, y, color) {
    setPromotion(false);
}
/// destruktor
Pawn::~Pawn() = default;
/*
 * Funkcja przyjmuje 4 parametry:
 * - Roznica miedzy indeksem figury a indeksem docelowego pola,
 * - indeks aktualnej pozycji figury
 * - indeks pola docelowego
 * - stan gry
 * Jesli ta figura nie ma dostepnego bicia, to wchodzimy do kolejnego warunku:
 * Jesli wartosc bezwzgledna roznicy miedzy indeksami wynosi 7 lub 9 (ruch po przekatnej o 1 pole), docelowe pole jest puste,
 * oraz jest to ruch do przodu (ostatnia linijka wartunku) zwracana jest prawda
 */
bool Pawn::forwardMoveCheck(int roznica, int curPos, int pole, const StatePtr& stan) {

    if (!checkIfThisPieceIsAbleToCapture()) {
        if (((std::abs(roznica) == 7 || std::abs(roznica) == 9) && stan->getPieceInformation(curPos + roznica) == 0)
            && ((pole > curPos && getColor() == 0) || (pole < curPos && getColor() == 1)))
            return true;
    }
    return false;
}
/*
 * Funkcja przyjmuje 4 parametrow:
 * - Roznica miedzy indeksem figury a indeksem docelowego pola,
 * - indeks aktualnej pozycji figury
 * - indeks pola docelowego
 * - stan gry
 * Jesli wartosc bezwzgledna roznicy wynosi 18 lub 14, sprawdzane jest, czy ruch jest do przodu (2 linijka)
 * Jesli tak to sprawdzane jest, czy pole o indeksie curPos+1/2roznicy zawiera figure przeciwnika i czy pole o indeksie curPos+roznica jest puste
 * Jesli tak - zwroc prawde
 * Jesli jest to ruch do tylu, sprawdzany jest analogiczny warunek, z tym, ze odejmujemy roznice, a nie dodajemy do curPos
 */
bool Pawn::captureMoveCheck(int roznica, int curPos, int pole, const StatePtr& stan) {
    if (std::abs(roznica) == 18 || std::abs(roznica) == 14) {
        if ((curPos < pole && getColor() == 0) || (curPos > pole && getColor() == 1)) {     // warunek ruchu do przodu
            return (((stan->getPieceInformation(curPos + (roznica / 2)) == 3 - 2 * getColor()
                      || stan->getPieceInformation(curPos + (roznica / 2)) == 4 - 2 * getColor())
                     && stan->getPieceInformation(curPos + roznica) == 0) == 1);
        }
        else return (((stan->getPieceInformation(curPos - (roznica / 2)) == 3 - 2 * getColor()
                       || stan->getPieceInformation(curPos - (roznica / 2)) == 4 - 2 * getColor())
                      && stan->getPieceInformation(curPos - roznica) == 0) == 1);
    }
    return false;
}
/*
 * Funkcja otrzymuje aktualny stan oraz indeks pola docelowego, ktory domyslnie przyjmuje wartosc -1
 * (sluzy to do funkcji checkIfThisPieceCanMove, ktora dla danej figury sprawdza dla kazdego pola, czy moze sie ruszyc)
 * W celu weryfikacji ruchow, wykorzystywane sa metody forwardMoveCheck i captureMoveCheck ruch jest legalny, zwroc prawde
*/
bool Pawn::checkIfThisMoveIsPossible(StatePtr stan, int pole) {
    if (pole==-1) pole = stan->convertStringToArrayIndex(stan->getPlayedMove());    // ustawianie odpowiedniego pola, w przypadku braku podania go domyslnie
    // (stan->convertStringToArrayIndex(stan->getPlayedMove()) przyjmuje tylko wartosci od 0 do 63 ze wzgledu na ograniczenia stringa playedMove)
    int curPos = 8*(getY()-1)+getX()-1;     // indeks aktualnej pozycji figury, poprzez odpowiednia konwersje x i y
    int roznica=std::abs(pole-curPos);       // wartosc bezwzgledna roznicy miedzy indeksami
    if (roznica==7 || roznica == 9 || roznica == 14 || std::abs(roznica) == 18) {  // jedyne wartosci, ktore maja sens dla pionka - 1 lub 2 pola po przekatnej
        if (getColor()==1) roznica=-roznica;                    // konwersja na liczbe przeciwna dla czarnych pionkow - ida one na odwrot (do przodu, gdy roznica jest ujemna)
        switch (getX()) {
            case (1):
            case (8):   // x = 1 lub x = 8 - pionki bandowe
                if (pole % 8 == 1 || pole % 8 == 6)     // pionki bandowe po ruchu do przodu moga wyladowac tylko na linii B lub G - odrzucenie ewentualnych nielegalnych ruchow
                    return (forwardMoveCheck(roznica, curPos, pole, stan));
                else if (pole % 8 == 2 || pole % 8 == 5)  // pionki bandowe po zbiciu moga wyladowac tylko na linii C lub F - odrzucenie  ewentualnych nielegalnych ruchow
                    if (captureMoveCheck(roznica, curPos, pole, stan)) { setIsAbleToCapture(true) ; return true; } // figura mogla to zbic - ustaw odpowiednie pole poza zwroceniem prawdy
                break;
            case (2):
            case (7):
                if (pole % 8 == 3 || pole % 8 == 4 ) {  // pionki B i G po zbiciu moga wyladowac tylko na linii D lub E - odrzucenie nielegalnych ruchow
                    if (captureMoveCheck(roznica, curPos, pole, stan)) {
                        setIsAbleToCapture(true);
                        return true;
                    } // figura mogla to zbic - ustaw odpowiednie pole poza zwroceniem prawdy
                }
                else return (forwardMoveCheck(roznica, curPos, pole, stan));
            default: {
                if (forwardMoveCheck(roznica, curPos, pole, stan))
                    return true; // pionki b-g nie maja zadnych ograniczen przy ruchach do przodu. Natomiast dla pionkow c-f trzeba sprawdzic jeszcze jeden warunek ponizej
                if (getX() >= 3 && getX() <= 6) {    // pionki c-f nie maja zadnych ograniczen przy biciu
                    if (captureMoveCheck(roznica, curPos, pole, stan)) { setIsAbleToCapture(true) ; return true; } // figura mogla to zbic - ustaw odpowiednie pole poza zwroceniem prawdy
                }
                break;
            }
        }
    }
    return false;       // zwroc falsz, jesli powyzsze warunki nie zostaly spelnione
}



