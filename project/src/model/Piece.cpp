#include "Piece.h"
#include "State.h"
#include "Player.h"
#include "Queen.h"
/// ustawiamy x, y i kolor - najwazniejsze informacje o figurze. Promocje ustala konstruktor pionka/krolowej
/// natomiast przy tworzeniu figury zakladamy, ze nie ma ona bicia, bo to zalezy od stanu gry
/// i z poczatkiem tury gracza jest sprawdzane dla kazdej figury, czy moze ona bic
Piece::Piece(int x, int y, bool color) : x(x), y(y), color(color) {
    setIsAbleToCapture(false);
}
/// getter aktualnej pozycji X pionka
int Piece::getX() const {
    return x;
}
/// setter nowej pozycji X pionka
void Piece::setX(int newX) {
    Piece::x = newX;
}
/// getter aktualnej pozycji Y pionka
int Piece::getY() const {
    return y;
}
/// setter nowej pozycji Y pionka
void Piece::setY(int newY) {
    Piece::y = newY;
}
/// setter koloru figury - prawda dla czarnych, falsz dla czarnych
bool Piece::getColor() const {
    return color;
}
/// setter koloru figury - prawda dla czarnych, falsz dla czarnych
void Piece::setColor(bool newColor) {
    Piece::color = newColor;
}
/// getter promocji figury - prawda dla krolowej, falsz dla pionka
bool Piece::getPromotion() const {
    return isPromoted;
}
/// setter promocji figury - prawda dla krolowej, falsz dla pionka
void Piece::setPromotion(bool newIsPromoted) {
    Piece::isPromoted = newIsPromoted;
}
/// getter informacji o tym, czy figura moze zbijac
bool Piece::checkIfThisPieceIsAbleToCapture() const {
    return isAbleToCapture;
}
/// setter informacji o tym, czy figura moze zbijac
void Piece::setIsAbleToCapture(bool newIsAbleToCapture) {
    Piece::isAbleToCapture = newIsAbleToCapture;
}
/// destruktor
Piece::~Piece() = default;
/*
 * Metoda do zbijania pionka przeciwnika, przyjmujaca wskaznik do stanu gry, wywolywana wewntarz ruchu gracza
 * Pobieramy ze stanu pole wybranej figury i zagrany ruch i konwertujemy wartości na indeks w tablicy currentPosition
 * Następnie, przeskakujemy pole po polu po przekątnej od aktualnej pozycji do docelowej pozycji, i szukamy po kordynatach figury przeciwnika.
 * W sytuacji, gdy metoda findPiece z wektora przeciwnika zwroci wskaznik do figury, to ja usuwamy z wektora, oraz aktualizujemy currentPosition dla pola zajmowane przez tą figurę na puste
 */
void Piece::killOpponentPiece(const StatePtr& stan) const {
    int pole = stan->convertStringToArrayIndex(stan->getPlayedMove());      // indeks docelowego pole, po konwersji ze stringa za pomoca funkcji convertStringToArrayIndex
    int curPos = stan->convertStringToArrayIndex(stan->getSelectedPieceString()); // indeks pozycji startowej figury, po konwersji ze stringa za pomoca funkcji convertStringToArrayIndex
    int przeskok;       // jedno pole po przekatnej w strone docelowego pola, ustawiane ponizej
    if (((pole-curPos) % 7) == 0) przeskok = 7; else przeskok = 9;  // pole - curPos moze byc tylko podzielne przez 7 lub 9 - zatem ustawia to na jedna z tych wartosci
    if (pole<curPos) przeskok=-przeskok;        // ruch do tylu - ustaw przeskok na przeciwna wartosc
    int y_pos; int x_pos;       // x, y - kordynaty ustawiane wewnatrz petli
    PiecePtr pieceToDelete = nullptr;    // zalozenie poczatkowe - nie ma figury do usuniecia
    while (pieceToDelete == nullptr) {  // program jest tak napisany, ze ta figura kiedys sie znajdzie - a przynajmniej mam taka nadzieje
        curPos+=przeskok;               // przeskocz do kolejnego pola po przekatnej w strone docelowego pola
        y_pos = (curPos + 8) / 8; x_pos = (curPos % 8) + 1;     // odpowiednia konwersja y i x z indeksu nowej pozycji
        if (getColor()) pieceToDelete=stan->getWhite()->findPiece(stan->getWhite()->getPiecesLeft(),x_pos,y_pos);   // jesli figura byla czarna, szukaj w wektorze bialych po x i po y
        else pieceToDelete=stan->getBlack()->findPiece(stan->getBlack()->getPiecesLeft(),x_pos,y_pos);  // jesli figura byla biala, szukaj w wektorze czarnych po x i po y
    }
    stan->changePieceInformation((pieceToDelete->getY()-1)*8+pieceToDelete->getX()-1, 0);       // zmien informacje w miejscu figury do usuniecia na 0 (puste pole)
    // indeks jest ustalany poprzez odpowiednia konwersje aktualnego y i x figury do usuniecia
    if (getColor()) {      // jesli figura byla czarna, szukaj w wektorze bialych indeksu figury do usuniecia, a potem usun ja z wektora
        for (std::size_t i = 0; i < stan->getWhite()->getPiecesLeft().size(); i++) {
            if (stan->getWhite()->getPiecesLeft()[i] == pieceToDelete) {
                stan->getWhite()->getPiecesLeft().erase(stan->getWhite()->getPiecesLeft().begin() + i);
            }
        }
    }
    else {      // jesli figura byla biala, szukaj w wektorze czarnych indeksu figury do usuniecia, a potem usun ja z wektora
        for (std::size_t i = 0; i < stan->getBlack()->getPiecesLeft().size(); i++) {
            if (stan->getBlack()->getPiecesLeft()[i] == pieceToDelete) {
                stan->getBlack()->getPiecesLeft().erase(stan->getBlack()->getPiecesLeft().begin() + i);
            }
        }
    }
}
/*
 * Metoda do ruszania figur, przyjmuje wskaznik do aktualnego stanu.
 * Zmieniamy informacje w currentPosition o aktualnej pozycji figury na 0 - puste pole, oraz na docelowej pozycji w zależności od tego, jaka figura wykonywała ruch - odpowiednie pola są pobierane w celu ustalenia tego faktu
 * Następnie, dokonujemy odpowiednią konwersje indeksu docelowego ruchu na x i y i w celu ustawienia nowych wartości koordynatów figury.
 */
void Piece::move(const StatePtr& stan) {
    stan->changePieceInformation(8*(getY()-1)+getX()-1,0);      // 8*(y-1)+x-1 to konwersja na indeks w tablicy - zmieniamy informacje na to, ze pole teraz jest puste
    stan->changePieceInformation(stan->convertStringToArrayIndex(stan->getPlayedMove()),1+getPromotion()+2*getColor());
    /*
     * stan->convertStringToArrayIndex(stan->getPlayedMove()) to po prostu wpisany ruch po konwersji na liczbe calkowita
     * ustawiamy informacje na nowym polu na 1 + (0 dla pionka, 1 dla krolowej) + (0 dla bialych, 2 dla czarnych)
     * A ponizej zmieniamy polozenie naszego pionka na nowe przez odpowiednie operacje:
     * Nowy x to reszta z dzielenia przez 8 + 1, nowy y to calkowita czesc indeksu powiekszonego o 8 i podzielonego przez 8 (np dla indeksu 7: x = 7%8+1 = 8, y = 15/8=1
     */
    setX((stan->convertStringToArrayIndex(stan->getPlayedMove()) % 8) + 1); setY ((stan->convertStringToArrayIndex(stan->getPlayedMove()) + 8) / 8);
}
/*
 * Promowanie figur - metoda przyjmuje wskaźnik do aktualnego stanu gry
 * Jeśli figura jest na ostatniej linii (y=8 dla białej, y=1 dla czarnej) i jest pionkiem, to pionek szuka sama siebie w wektorze gracza, który nim steruje.
 * Następnie, usuwa się z wektora, do którego dodawany jest królowa o tych samych własnościach - x, y i kolorze.
 * Na samym końcu, aktualizujemy currentPosition w stanie gry - zastępujemy pionka królową.
 */
void Piece::promoteIfOnBackRank(const StatePtr& stan) {
    if (((getColor()==0 && getY()==8) || (getColor()==1 && getY()==1)) && !getPromotion()) {
        if (getColor()) {       // jesli figura jest czarna
            for (std::size_t i = 0; i < stan->getBlack()->getPiecesLeft().size(); i++) {        // przeszukaj caly wektor czarnych
                if (stan->getBlack()->getPiecesLeft()[i] == shared_from_this()) stan->getBlack()->getPiecesLeft().erase(stan->getBlack()->getPiecesLeft().begin() + i);
                // jak figura sie znajdzie, to usuwa sie z wektora czarnego - troche jej szkoda
            }
            // do wektora czarnego dodajemy krolowa o tych samych wspolrzednych i kolorze, co figura na ostatniej linii
            stan->getBlack()->getPiecesLeft().push_back(std::make_shared<Queen>(getX(), getY(), getColor()));
            stan->changePieceInformation((getY() - 1) * 8 + getX() - 1, 4);
            // zmieniamy informacje o tym polu na 4 - czarna krolowa
        }
        else { // ta sama logika, co dla czarnych, tyle ze dla bialych
            for (std::size_t i = 0; i < stan->getWhite()->getPiecesLeft().size(); i++) {
                if (stan->getWhite()->getPiecesLeft()[i] == shared_from_this()) stan->getWhite()->getPiecesLeft().erase(stan->getWhite()->getPiecesLeft().begin() + i);
            }
            stan->getWhite()->getPiecesLeft().push_back(std::make_shared<Queen>(getX(), getY(), getColor()));
            stan->changePieceInformation((getY()-1)*8+getX()-1,2); // 2 - biala krolowa
        }
    }
}
/*
 * Metoda przyjmuje wskaźnik do aktualnego stanu gry. Dla każdego możliwego sprawdzamy, czy ruch jest możliwy.
 * Jeśli jakikolwiek ruch był możliwy, zwracamy od razu prawdę.
 * Jeśli przejdziemy przez wszystkie pola i nie znajdziemy legalnego ruchu - zwracamy fałsz.
 */
bool Piece::checkIfThisPieceCanMove(const StatePtr& stan) {
    for (int i=0; i<=63; i++) {
        if (checkIfThisMoveIsPossible(stan,i)) return true;
    }
    return false;
}




