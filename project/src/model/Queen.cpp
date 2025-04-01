#include "Queen.h"
#include "State.h"
/// Konstruktor dla królowej. Ustawia on pole setPromotion na true - bo królowa jest po promocji
Queen::Queen(int x, int y, bool color) : Piece(x, y, color) {
    setPromotion(true);
}
/// destruktor
Queen::~Queen() = default;
/*
 * Funkcja otrzymuje aktualny stan oraz indeks pola docelowego, ktory domyslnie przyjmuje wartosc -1
 * (sluzy to do funkcji checkIfThisPieceCanMove, ktora dla danej figury sprawdza dla kazdego pola, czy moze sie ruszyc)
 * Jeśli ruch jest legalny, zwroc prawde
*/
bool Queen::checkIfThisMoveIsPossible(StatePtr stan, int pole) {
    if (pole==-1) pole = stan->convertStringToArrayIndex(stan->getPlayedMove());           // taka sama logika jak dla pionka
    int y = (pole + 8) / 8; int x = (pole % 8) + 1;             // konwersja indeksu pola na odpowiednie x i y
    if ((x-y==getX()-getY() || 9-x-y==9-getX()-getY()) && (getX()!=x) && stan->getPieceInformation(pole)==0) {
        /*
         * powyzej tak zwany warunek konieczny - ruch po przekatnej, na inne, puste pole, niz dotychcasowe
         * aby ruch byl po przekatnej, roznica miedzy x a y przed musi byc taka sama jak roznica miedzy x a y po,
         * lub roznica pomiedzy (9-x) a y przed musi byc taka sama jak roznica pomiedzy (9-x) a y po
         * Najlatwiej to zrozumiec, rozrysowujac to odpowiednio w excelu - ja tak na to wpadlem przynajmniej
         * No i oczywiscie, kordynaty musza sie zmienic - wystarczy sprawdzic to dla jednego z nich (w tym przypadku x)
         * oraz docelowe pole musi byc puste
         */
        int curPos = 8*(getY()-1)+getX()-1;     // indeks aktualnej pozycji figury uzyskiwany poprzez odpowiednia konwersje x i y
        if (((pole-curPos) % 7) == 0) y = 7; else y = 9;
        if (pole<curPos) y=-y;
        /*
         * Wykorzystywanie zmiennych deklarowanych wczesniej, bo sa juz one "niepotrzebne"
         * y jest wartoscia roznicy indeksu 1 pola po przekatnej od aktualnej pozycji figury do pozycji docelowej
         * Jedyne wartosci, jakie moga "wpasc" to podzielne przez 7 lub 9, zatem w zaleznosci od tego, przez co roznica jest podzielna, tak ustawiamy y
         * I oczywiscie, jesli indeks pola docelowego jest mniejszy od indeksu aktualnej pozycji figury, ustawiamy wartosc na przeciwna - ruch do tylu
         * Natomiast x bedzie stanowil licznik napotkanych figur przeciwnika na drodze - moze wyniesc 0 albo 1, w przeciwnym razie ruch jest nielegalny
         */
        x=0;
        while (curPos!=pole) {          // dopoki nie przejde na pole docelowe
            curPos+=y;                  // sprawdzanie n-tego pola po przekatnej
            if (stan->getPieceInformation(curPos) == 3 - 2 * !getColor() || stan->getPieceInformation(curPos) == 4 - 2 * !getColor()) return false;
            if (stan->getPieceInformation(curPos) == 3 - 2 * getColor() || stan->getPieceInformation(curPos) == 4 - 2 * getColor()) x++;
            if (x>1) return false;
            /*
             * Konwersja {3,4} - 2 * !getColor() wyszuka figury swoje, natomiast konwersja {3,4} - 2 * getColor() wyszuka figury przeciwnikia
             * Przyklad - mamy figure biala, wiec {3,4} - 2 * !0 = {3,4} - 2*1 = {1,2}
             * Jesli na drodze napotkamy swoja figure, automatycznie zwroc falsz
             * Natomiast jesli spotkamy figure przeciwnika, zwieksz licznik figur przeciwnika o 1
             * Na koncu sprawdzamy, czy krolowa napotkala na drodze wiecej niz jedna figure przeciwnika. Jesli tak, automatycznie zwroc falsz
             */
        }
        if (x==1) setIsAbleToCapture(true);      // krolowa napotkala tylko 1 droge przeciwnika - wiec moze zbic
        if (x==0 && checkIfThisPieceIsAbleToCapture()) return false;        // jesli krolowa moze zbic, ale po drodze nie znalazla zadnej figury przeciwnika, zwroc falsz
        return true;        // Jesli jakims cudem udalo sie dotrzec tutaj, to oznacza, ze x=0 i krolowa nie moze zbic, lub x=1 i krolowa moze zbic. Wtedy zwroc prawde
    }
    return false;       // jesli udalo sie tutaj dojsc, to znaczy, ze ruch jest nielegalny - zwroc falsz
    /*
     * Ciekawostka:
     * Spodziewalem sie, ze wymyslenie i napisanie logiki krolowej bedzie duzo ciezsze niz pionka
     * W rzeczywistosci, bylo to duzo prostsze i przyjemniejsze
     */
}
