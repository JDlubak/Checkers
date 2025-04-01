#include <ctime>
#include "Computer.h"
#include "State.h"
#include "Piece.h"
/// konstruktor - ustawiamy setHuman false
Computer::Computer(const std::string &name, bool playedColor, const std::vector<PiecePtr> &piecesLeft) : Player (name,
                                                                                                                playedColor,
                                                                                                                piecesLeft) {
    setIsHuman(false);
}
/// destruktor
Computer::~Computer() = default;
/// Metoda otrzymuje wskaźnik do aktualnego stanu gry. Komputer losuje jedną ze swoich figur, a następnie jest sprawdzane, czy to poprawny wybór (figura może się poruszać)
/// W momencie ustalenia poprawności wyboru figury, przechodzimy do fazy 2 - wylosowania pola docelowego. W zależności od typu figury, oraz możliwości zbicia, możliwości wyboru są ograniczane.
/// Po wylosowaniu prawidłowego ruchu zostaje on wykonany - wywoływana jest metoda move dla figury, w celu poruszenia się
/// W przypadku, gdy dana figura może zbijać, wywoływana jest metoda move tyle razy, ile jest możliwych bić, i po każdym ruchu następuje zbicie figury poprzez wywołanie metody killOpponentPiece
void Computer::makeMove(StatePtr stan) {

    srand(time(nullptr));                       // Inicjalizacja generatora liczb losowych
    int k;          // deklaracja zmiennej odpowiedzialnej za przechowywanie wylosowanego wyniku
    bool loopController;                // "kontroler pętli' - zmienna, ktora po ustawieniu na wartosc true przerywa petle, a gdy pozostaje falszen, oznacza to, ze byl blad w wyborze figury.
    while (true) {
        // Komputer dziala w inny sposob niz czlowiek przy wyborze figury, bo losuje figure, a nie pole
        // Jest to spowodowane faktem, ze figure mozna wybrac na maksymalnie 12 sposobow, a pole na 64, wiec jest to duzo wydajniejsze
        k = rand() % getPiecesLeft().size();            // losujemy indeks figury. k bedzie losowa liczba z przedzialu <0;y>, gdzie y - ilosc figur w wektorze komputera
        stan->setSelectedPiece(getPiecesLeft()[k]);         // wybranie figury o indeksie k z wektora  i ustawienie jej
        /*
         * Ponizej zachodzi odpowiednia konwersja kordynatow wylosowanej figury na pole w formacie "a1" (litera a-h i cyfra 1-8), na ktorym sie znajduje, znaczenie:
         * Pierwszy znak ustalamy z przedzialu a-h. W tym celu do znaku ' (96 z tablicy ASCII, znajduje sie on bezposrednio przed 'a' - 97 z tablicy ASCII)
         * dodajemy aktualny x i tworzymy string z pojedynczego takiego znaku
         * Drugi znak tworzymy poprzez konwersje "to_string" wspolrzednej y wybranej figury
         * Obydwa znaki laczymi w jednego stringa, ktorego przesylamy do metody odpowiedzialnej za ustawienie tego pola
        */
        stan->setSelectedPieceString(std::string(1,'`' + stan->getSelectedPiece()->getX()) + std::to_string(stan->getSelectedPiece()->getY()));
        // zmien wartosc loopControllera w zaleznosci od tego, czy wpisano dobre pole figury
        // isUserGenerated: 0 - nie wyswietlaj komunikatow bledu
        // whichPartNeedsToBeVerified: 1 - metoda weryfikuje obydwie fazy ruchu, zatem informujemy ja, ze chodzi o wybor figury
        loopController = moveVerification(stan, true);
        if (loopController) break;  // jesli wygenerowana figura przeszla test metody, przerwij petle i przejdz do wyboru pola docelowego
    }
    while (true) {
        int tab[28] = {-7,7,9,-9,
                       14,-14,18,-18,
                       21,-21,27,-27,
                       28,-28,36,-36,
                       35,-35,45,-45,
                       42,-42,54,-54,
                       49,-49,63,-63};
        /*
         * Aby usprawnic dzialanie losowania docelowego pola, utworzona zostala powyzsza tablica,
         * Sa to wszystkie mozliwe roznice miedzy indeksem pola aktualnego a indeksem pola docelowego
         * W ten sposob losujemy tylko z 28 pozycji, a nie z calej planszy
         */
        k = stan->getSelectedPiece()->getPromotion() ? stan->getSelectedPiece()->checkIfThisPieceIsAbleToCapture() ? rand() % 24 + 4 : rand() % 28
                                                     : stan->getSelectedPiece()->checkIfThisPieceIsAbleToCapture() ? rand() % 4 + 4 : rand() % 4;
        /*
         * Kolejne usprawnienie losowania. Rozpatrywane sa 2 warunki - promocja figury i to, czy moze zbijac.
         * Jesli jest krolowa i moze zbijac, obcinane sa pierwsze 4 elementy tablicy - nie ma sensu sprawdzac, czy moze isc o 1 pole po przekatnej przeciez
         * Jesli jest krolowa i nie moze zbijac, to losujemy z calego przedzialu.
         * Wieksze znaczenie ma to jednak dla pionkow.
         * Jesli pionek moze zbijac, to losujemy sposrod 4 elementow znajdujacych sie na indeksach 4-7 - sa ta mozliwe wartosci roznicy zbicia.
         * Jesli pionek nie moze zbijac, to losujemy sposrod pierwszych 4 elementow w tablicy - czyli ruch o 1 pole po przekatnej
         * W ten sposob, w szczegolnosci dla pionka, mozemy znaczaco usprawnic losowanie
         */
        int x, y; // dla wiekszej czytelnosci kodu deklarujemy 2 zmienne, ktore wykorzystamy przy konwersji indeksu na stringa
        x = stan->convertStringToArrayIndex(stan->getSelectedPieceString());        // tymczasowo wykorzystujemy x w innym celu
        if (!(x+tab[k]>63 || x+tab[k] < 0)) {
            y = ((x+tab[k]) + 8) / 8; x = ((x+tab[k]) % 8) + 1;  // konwersja sumy indeksow roznicy i aktualnej pozycji pionka na kordynaty docelowe
            stan->setPlayedMove(std::string(1,'`' + x )+ std::to_string(y));        // i ustawienie pola docelowego przy wykorzystaniu schematu powyzszego
        }
        // Jesli wygenerowane pole znajduje sie poza zakresem, to ustaw aktualna pozycje figury jako pole docelowe - zostanie to szybko odrzucone i tak
        else stan->setPlayedMove(std::string(1,'`' + stan->getSelectedPiece()->getX()) + std::to_string(stan->getSelectedPiece()->getY()));
        /*
         * Oczywiscie przy takiej implementacji generacja nielegalnych ruchow dalej jest mozliwa
         * Przykladowo, pionek stojacy na polu a3 (indeks 16), ktory moze zbijac moze wylosowac wartosc 14, co po zsumowaniu odpowiada polu g4 (bo ono posiada indeks 30)
         * Natomiast takie przypadki powinny juz odrzucac odpowiednie metody
         */

        // zmien wartosc loopControllera w zaleznosci od tego, czy wpisano dobre pole figury
        // isUserGenerated: 0 - nie wyswietlaj komunikatow bledu
        // whichPartNeedsToBeVerified: 0 - metoda weryfikuje obydwie fazy ruchu, zatem informujemy ja, ze chodzi o wybor pola docelowego
        loopController = moveVerification(stan, false);
        if (loopController) {   // jesli wygenerowane pole przeszlo test metody, rozpatrz kolejny warunek ponizej
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
         * Mamy 2 warunki zakonczenia petli - wygenerowane pole przeszlo pole metody oraz figura juz nie moze zbijac
         * Zakladajac poprawne wpisane docelowe, mamy 2 mozliwosci:
         * a) figura nie moze zbijac i b) figura moze zbijac, rozpatrujac je mamy:
         * a) po ruchu figura "dalej nie moze zbijac" - bo program nie wywoluje metody weryfikujacej to
         * b) po ruchu jest sprawdzany warunek, czy figura moze dalej zbijac: jesli moze, to program przejdzie petle jeszcze raz, a jesli nie, to petla sie zakonczy
         * Dlatego nie ma szans na sytuacje, gdzie np. w pierwszym ruchu ruszamy sie pionkiem o 1 pole, ale nagle moze zbijac (z punktu widzenia czlowieka) i zaczynamy zbijanie
         */
    }
}



