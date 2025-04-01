#include <boost/test/unit_test.hpp>
#include "typedefs.h"
#include "Pawn.h"
#include "Queen.h"
#include "Computer.h"
#include "State.h"

/*
 * Tutaj znajduje sie testy dla figur. Poniewaz mechanika ruchow wykorzystuje metody innych klas, to rowniez efekty tych metod zostaly poddane weryfikacji
 * Za generowanie ruchow odpowiada gracz komputerowy, ktory w kazdym tescie ma do wyboru tylko jedna figure i jeden mozliwy ruch do wykonania - w przeciwnym razie wyniki by byly rozne
 * Wszystkie metody figur zostaly przetestowane - niektore posrednio, tak jak np. forwardMoveCheck i captureMoveCheck, ktore metoda checkIfThisMoveIsPossible wykorzystuje w celu poprawy czytelnosci kodu
 */

struct TestSuitePiecesFixture {
    PiecePtr b1 = std::make_shared<Pawn>(0, 1, 0);
    PiecePtr c1 = std::make_shared<Pawn>(4, 5, 1);
    std::vector<PiecePtr> whitePieces;
    std::vector<PiecePtr> blackPieces;
    PlayerPtr testWhite = std::make_shared<Computer>("testPlayer", 0, whitePieces);
    PlayerPtr testBlack = std::make_shared<Computer>("testPlayer2", 1, blackPieces);
    int testCurrentPosition[64] = {     // pozycja reprezentujaca polozenie figur, z jakiego bedziemy rozpatrywac testy
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 3,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
    };
    StatePtr stan = std::make_shared<State>(0, testCurrentPosition, testWhite, testBlack);
    TestSuitePiecesFixture() {}
    ~TestSuitePiecesFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TestSuite, TestSuitePiecesFixture)

    BOOST_AUTO_TEST_CASE(PiecesAssertionsTests) {
        BOOST_TEST(stan->getWhite()->getPiecesLeft().size() == 0);                  // ustawialismy puste wektory
        BOOST_TEST(stan->getBlack()->getPiecesLeft().size() == 0);
        stan->getWhite()->getPiecesLeft().push_back(b1);                            // dodajemy obie figury do poszczegolnych wektorow
        stan->getBlack()->getPiecesLeft().push_back(c1);
        BOOST_TEST(stan->getWhite()->getPiecesLeft().size() == 1);                  // i sprawdzamy, czy sie dodaly
        BOOST_TEST(stan->getBlack()->getPiecesLeft().size() == 1);
        /*
         * Najpierw testujemy poprawnosc ustawienia wszystkich pol
         */
        BOOST_TEST(b1->getX()==0); BOOST_TEST(b1->getY()==1);             // podano x=0,y=1
        BOOST_TEST(b1->getColor()==0);          // bialy pionek
        BOOST_TEST(b1->getPromotion()==0);      // zwykly pionek
        BOOST_TEST(c1->getX()==4); BOOST_TEST(c1->getY()==5);             // podano x=4,y=5
        BOOST_TEST(c1->getColor()==1);          // czarny pionek
        BOOST_TEST(c1->getPromotion()==0);      // zwykly pionek
        b1->setX(1); b1->setY(3), c1->setX(8); c1->setY(6);              // przestawiamy na pola pokrywajace sie z currentPosition
        BOOST_TEST(b1->getX()==1); BOOST_TEST(b1->getY()==3);            // i jeszcze raz testujemy, czy wszystko dziala jak nalezy
        BOOST_TEST(c1->getX()==8); BOOST_TEST(c1->getY()==6);            // i jeszcze raz testujemy, czy wszystko dziala jak nalezy
        b1->setColor(1); c1->setColor(0);               // no w praktyce tak nie wolno, ale przetestowac trzeba
        b1->setPromotion(1); c1->setPromotion(1);       // a tego tym bardziej (bo do tego kasujemy pionka i tworzymy krolowa)
        BOOST_TEST(b1->getColor()); BOOST_TEST(!c1->getColor());        // sprawdzamy
        BOOST_TEST(b1->getPromotion()); BOOST_TEST(c1->getPromotion());           // to tez
        b1->setColor(0); c1->setColor(1);                               // i wracamy do normalnosci
        b1->setPromotion(0); c1->setPromotion(0);   // i tutaj tez wracamy
        /*
         * Nastepnie sprawdzamy podstawowe ruchy. Wstepnie bialy pionek znajduje sie na a3, a czarny na h6.
         */
        BOOST_TEST_REQUIRE(b1->checkIfThisPieceCanMove(stan));  // oba pionki powinny umiec sie poruszac
        BOOST_TEST_REQUIRE(c1->checkIfThisPieceCanMove(stan));
        stan->setPlayedMove("b4");                              // zaloz, ze uzytkownik/komputer podal b4
        BOOST_TEST(b1->checkIfThisMoveIsPossible(stan));        // b1 moze isc na b4 - sprawdz to
        BOOST_TEST(!c1->checkIfThisMoveIsPossible(stan));       // c1 nie moze isc na b4 - sprawdz to
        stan->setPlayedMove("b2");                              // zaloz, ze uzytkownik/komputer podal b2
        BOOST_TEST(!b1->checkIfThisMoveIsPossible(stan));       // pionek nie moze isc do tylu - sprawdz to
        stan->setPlayedMove("h3");                              // pionek znajduje sie na a3 o indeksie 16, h3 ma indeks 23
        BOOST_TEST(!b1->checkIfThisMoveIsPossible(stan));       // pomimo roznicy indeksu 7, pionek nie moze tak chodzic - sprawdz to
        stan->setPlayedMove("g5");                              // zaloz, ze uzytkownik/komputer podal g5
        BOOST_TEST(!b1->checkIfThisMoveIsPossible(stan));       // b1 nie moze isc na g5 - sprawdz to
        BOOST_TEST(c1->checkIfThisMoveIsPossible(stan));        // c1 moze isc na g5 - sprawdz to
        stan->setPlayedMove("g7");                              // zaloz, ze uzytkownik/komputer podal g7
        BOOST_TEST(!c1->checkIfThisMoveIsPossible(stan));       // pionek nie moze isc do tylu - sprawdz to
        stan->setPlayedMove("a6");                              // pionek znajduje sie na h6 o indeksie 47, a6 ma indeks 40
        BOOST_TEST(!c1->checkIfThisMoveIsPossible(stan));       // pomimo roznicy indeksu 7, pionek nie moze tak chodzic - sprawdz to
        /*
         * Teraz przestawimy oba pionki i przetestujemy dzialanie changePieceInformation i getPieceInformation ze stanu
         */
        BOOST_TEST(stan->getPieceInformation(47)==3);           // czarny pionek tu jest
        BOOST_TEST(stan->getPieceInformation(16)==1);           // bialy pionek tu jest
        testWhite->makeMove(stan);                                    // popros komputer o ruch
        b1->promoteIfOnBackRank(stan);                      // probujemy dokonac promocji figury - nie uda sie to
        BOOST_TEST_REQUIRE(b1 == stan->getWhite()->getPiecesLeft()[0]);         // sprawdzamy, czy 1 pionek w wektorze i nasz pionek to to samo - musi tak byc
        BOOST_TEST_REQUIRE(stan->getWhite()->getPiecesLeft().size() == 1);                 // upewniamy sie, ze nadal mamy tylko jednego pionka
        BOOST_TEST(!b1->getPromotion());
        BOOST_TEST(stan->getPlayedMove()=="b4");                      // jedyny mozliwy ruch to pionek z a3 na b4
        testBlack->makeMove(stan);                                    // popros komputer o ruch
        BOOST_TEST(stan->getPlayedMove()=="g5");                      // jedyny mozliwy ruch to pionek z h6 na g5
        BOOST_TEST(b1->getX()==2); BOOST_TEST(b1->getY()==4);         // nowe kordynaty - b4 - x=2,y=4
        BOOST_TEST(c1->getX()==7); BOOST_TEST(c1->getY()==5);         // nowe kordynaty - g5 - x=7,y=5
        BOOST_TEST(stan->getPieceInformation(47)==0);           // czarny pionek tu byl
        BOOST_TEST(stan->getPieceInformation(16)==0);           // bialy pionek tu byl
        BOOST_TEST(stan->getPieceInformation(25)==1);           // bialy pionek tu jest po ruchu - pole b4
        BOOST_TEST(stan->getPieceInformation(38)==3);           // bialy pionek tu jest po ruchu - pole g5
        /*
         * Teraz podstawimy figurom atrapy figur. Po prostu podmienimy 2 wartosci w currentPosition - w taki sposob pionki sprawdzaja, czy moga bic
         * Tyle ze te figury beda mylace - nasze figury nie beda mogly ich zbic, bo po zbiciu wyladowalyby za plansza - trzeba zwerfyfikowac, czy nie moga ich bic.
         */
        stan->changePieceInformation(32,4); // czarna krolowa na a5
        stan->changePieceInformation(31,2); // biala krolowa na h4
        BOOST_TEST(!b1->checkIfThisPieceIsAbleToCapture());     // domyslnie figury nie moga bic - sprawdzamy to
        BOOST_TEST(!c1->checkIfThisPieceIsAbleToCapture());
        for (int i=0;i<64;i++) {
            b1->checkIfThisMoveIsPossible(stan,i);              //aktualizujemy pole isAbleToCapture obu figurom
            c1->checkIfThisMoveIsPossible(stan,i);
        }
        BOOST_TEST(!b1->checkIfThisPieceIsAbleToCapture());     // no i teraz figury dalej nie powinny moc bic - sprawdzamy
        BOOST_TEST(!c1->checkIfThisPieceIsAbleToCapture());
        stan->changePieceInformation(32,0);     // usuwamy atrapy figur
        stan->changePieceInformation(31,0);
        // ale teraz podstawiamy nowe atrapy - te juz figury powinny moc zbic
        stan->changePieceInformation(18,4); // czarna krolowa na c3
        stan->changePieceInformation(29,2); // biala krolowa na f4
        for (int i=0;i<64;i++) {
            b1->checkIfThisMoveIsPossible(stan,i);              //aktualizujemy pole isAbleToCapture obu figurom
            c1->checkIfThisMoveIsPossible(stan,i);
        }
        BOOST_TEST(b1->checkIfThisPieceIsAbleToCapture());     // sprawdzamy, czy teraz figury moga bic
        BOOST_TEST(c1->checkIfThisPieceIsAbleToCapture());
        // Z tej pozycji mozna rozpatrywac 3 ruchy w przypadku kazdego pionka - 2 do przodu i bicie do tylu. Zweryfikujmy, ze jedynym dostepnym ruchem jest bicie
        BOOST_TEST_REQUIRE(!b1->checkIfThisMoveIsPossible(stan,32));     // ruch na a5
        BOOST_TEST_REQUIRE(!b1->checkIfThisMoveIsPossible(stan,33));     // ruch na c5
        BOOST_TEST_REQUIRE(b1->checkIfThisMoveIsPossible(stan,11));     // ruch z biciem na d2
        BOOST_TEST_REQUIRE(!c1->checkIfThisMoveIsPossible(stan,29));     // ruch na f4
        BOOST_TEST_REQUIRE(!c1->checkIfThisMoveIsPossible(stan,31));     // ruch na h4
        BOOST_TEST_REQUIRE(c1->checkIfThisMoveIsPossible(stan,20));     // ruch z biciem na e3
        /*
         * Podstawilismy atrapy, wszystko poszlo dobrze
         * Ale wypadaloby w tym miejscu umiescic prawdziwe figury i sprawdzic, czy sa zbijane, zatem:
         */
        stan->getBlack()->getPiecesLeft().push_back(std::make_shared<Queen>(3,3,1));  // ww. czarna krolowa na c3
        BOOST_TEST(stan->getBlack()->getPiecesLeft().size() == 2);      // sprawdzmy jeszcze raz rozmiar wektora
        testWhite->makeMove(stan);              // musi isc na d2 z biciem
        BOOST_TEST_REQUIRE(stan->getBlack()->getPiecesLeft().size() == 1);      // bicie musialo zajsc - sprawdzamy w ten sposob tez metode killOpponentPiece, wywolywana wewnatrz metody move
        BOOST_TEST(stan->getPlayedMove()=="d2");                // pionek musial tam isc
        BOOST_TEST(!b1->checkIfThisPieceIsAbleToCapture());     // po ruchu ta informacja sie resetuje - sprawdzamy
        stan->getWhite()->getPiecesLeft().push_back(std::make_shared<Queen>(6,4,0));  // ww. biala krolowa na f4
        BOOST_TEST(stan->getWhite()->getPiecesLeft().size() == 2); // sprawdzmy jeszcze raz rozmiar wektora
        testBlack->makeMove(stan);
        /*
         * Uwaga! Biala krolowa stala na f4, a bialy pionek jest na d2 po ruchu - co to oznacza?
         * Mamy podwojne bicie! Pionek najpierw zbija na e3, a potem laduje na c1 i promuje
         * (To nie bylo zamierzone dzialanie, siedzialem nad tym 10 minut i sie zastanawialem jakim cudem ten pionek wyladawal na c1...)
         */
        BOOST_TEST_REQUIRE(stan->getWhite()->getPiecesLeft().size() == 0);      // bicie musialo zajsc - sprawdzamy
        BOOST_TEST(stan->getPlayedMove()=="c1");                // pionek musial tam isc
        BOOST_TEST(!c1->checkIfThisPieceIsAbleToCapture());     // po ruchu ta informacja sie resetuje - sprawdzamy
        c1->promoteIfOnBackRank(stan);                          // probujemy dokonac promocji - uda sie to
        BOOST_TEST_REQUIRE(c1 != stan->getBlack()->getPiecesLeft()[0]);         // sprawdzamy, czy 1 pionek w wektorze i nasz pionek to to samo - nie moze tak byc
        BOOST_TEST_REQUIRE(stan->getBlack()->getPiecesLeft().size() == 1);                 // upewniamy sie, ze nadal mamy tylko jednego pionka
        c1 = stan->getBlack()->getPiecesLeft()[0];                  // ustawiamy naszego pionka na nowo utworzona krolowa
        BOOST_TEST(c1->getPromotion());
        BOOST_TEST(c1->getX() == 3);
        BOOST_TEST(c1->getY() == 1);
        BOOST_TEST(c1->getColor());        // sprawdzmy, czy pola zostaly odpowiednio przepisane
        /*
         * Znaczenie nastepnego testu: liczymy ilosc pol zajetych przez figury na szachownicy oraz wartosc. Powinna wyjsc 4 - na planszy mamy tylko czarna krolowa
         */
        int x=64,y=0;
        for (int i=0;i<64;i++) {
            if (stan->getPieceInformation(i)==0) x--;
            y+=stan->getPieceInformation(i);
        }
        BOOST_TEST(x==1);
        BOOST_TEST(y==4);               // poprawne wyniki oznaczaja, ze wszystko po ruchach prawidlowo sie ustawia na planszy
        /*
         * Podstawmy naszej krolowej jeszcze kolejnego pionka na g5. Jedyny legalny ruch w tym przypadku to jest bicie go i wyladowanie na h6
         */
        stan->changePieceInformation(38,1);
        stan->getWhite()->getPiecesLeft().push_back(std::make_shared<Pawn>(7,5,0));  // ww. biala krolowa na f4
        for (int i=0;i<64;i++) {
            c1->checkIfThisMoveIsPossible(stan,i);          // zaaktualizujmy informacje o tym, ze nasza krolowa moze bic
        }
        testBlack->makeMove(stan);
        BOOST_TEST(stan->getSelectedPiece()==c1);           // nie bylo sprawdzane do tej pory - sprawdzmy mechanike wyboru figury
        BOOST_TEST(stan->getPlayedMove()=="h6");
        BOOST_TEST(c1->getX() == 8);
        BOOST_TEST(c1->getY()== 6);
        /*
         * na koncu stworzymy z powrotem pionka bialego, ktorego postawimy na 7 linii i przetestujemy w ten sposob rowniez promocje dla bialego
         * Celem tych testow jest weryfikacja, ze nie mozemy zbijac wlasnych figur
         * Postawimy zatem atrapy figur blokujace nas i umozliwiajace tylko jeden legalny ruch
         * W przypadku bialych, umieszczamy pionka na polu c7, oraz atrape bialej krolowej na b8 i atrape bialego pionka na d6
         * W przypadku czarnych, ustawiamy 2 takowe pionki - jeden na polu g5, a drugi na f8
         * W ten sposob, ograniczamy ruch bialej figury tylko do d8 z promocja oraz tylko na g7 - co komputer moze wygenerowac
         * Z racji, ze tworzac atrapy, aktualizujemy tylko currentPosition, nie ma szans, by zostaly one wybrane
         */
        stan->changePieceInformation(57,2); // biala krolowa na b8
        stan->changePieceInformation(57,1); // bialy pionek na d6
        stan->changePieceInformation(38,3); // czarny pionek g5
        stan->changePieceInformation(61,3); // czarny pionek na f8
        stan->changePieceInformation(50,1); // i nasz "prawdziwy" pionek bialy
        b1=std::make_shared<Pawn>(3,7,0);                   // nasz pionek na c7
        stan->getWhite()->getPiecesLeft().push_back(b1);                // dodajemy go do wektora z powrotem
        for (int i=0;i<64;i++) {
            b1->checkIfThisMoveIsPossible(stan,i);              //aktualizujemy pole isAbleToCapture obu figurom
            c1->checkIfThisMoveIsPossible(stan,i);
        }
        BOOST_TEST_REQUIRE(!b1->checkIfThisPieceIsAbleToCapture());     // po swoich to chodzic nie bedziemy
        BOOST_TEST_REQUIRE(!c1->checkIfThisPieceIsAbleToCapture());
        testWhite->makeMove(stan);
        BOOST_TEST(stan->getSelectedPiece()==b1);           // sprawdzamy, czy odpowiednia figura zostala wybrana
        BOOST_TEST(stan->getPlayedMove()=="d8");            // sprawdzamy powyzsze zalozenie
        b1->promoteIfOnBackRank(stan);                      // dokonujemy promocji figury
        b1 = stan->getWhite()->getPiecesLeft()[0];          // ustawiamy naszego pionka na 1 pionka z wektora
        BOOST_TEST(b1->getPromotion());
        BOOST_TEST(b1->getX() == 4);
        BOOST_TEST(b1->getY() == 8);
        BOOST_TEST(!b1->getColor());        // sprawdzmy, czy pola zostaly odpowiednio przepisane
        BOOST_TEST(stan->getWhite()->getPiecesLeft().size() == 1);     // sprawdzamy jeszcze, czy w wektorze liczba pionkkow sie zgadza
        testBlack->makeMove(stan);
        BOOST_TEST(stan->getSelectedPiece()==c1);           // sprawdzamy, czy odpowiednia figura zostala wybrana
        BOOST_TEST(stan->getPlayedMove()=="g7");            // sprawdzamy powyzsze zalozenie
        BOOST_TEST(c1->getX() == 7);
        BOOST_TEST(c1->getY() == 7);
        BOOST_TEST(stan->getBlack()->getPiecesLeft().size() == 1);     // sprawdzamy jeszcze, czy w wektorze liczba pionkkow sie zgadza
        /*
         * Na zakonczenie, sprawdzmy jeszcze jedna rzecz - przestawmy czarna krolowa na 1 linie, i zobaczmy, co sie stanie, jesli sprobujemy ja wypromowac
         * Najpierw usunmy nasze atrapy, i przestawmy biala krolowa na b2, by wymusic ruch na a1
         */
        stan->changePieceInformation(57,0); // biala krolowa na b8
        stan->changePieceInformation(57,0); // bialy pionek na d6
        stan->changePieceInformation(38,0); // czarny pionek g5
        stan->changePieceInformation(61,0); // czarny pionek na f8
        stan->changePieceInformation(59,0); // przestawiamy nasza krolowa z d8
        stan->changePieceInformation(9,2); // na b2
        b1->setX(2); b1->setY(2);           // no i wewnatrz figury tez
        BOOST_TEST(b1->getX() == 2);
        BOOST_TEST(b1->getY() == 2);        // sprawdzmy poprawnosc ustawienia
        for (int i=0;i<64;i++) {
            c1->checkIfThisMoveIsPossible(stan,i);          // najpierw aktualizacja pola do bicia
        }
        BOOST_TEST_REQUIRE(c1->checkIfThisPieceIsAbleToCapture());          // i sprawdzamy, czy mozemy bic
        testBlack->makeMove(stan);              // i sie ruszamy
        BOOST_TEST_REQUIRE(stan->getWhite()->getPiecesLeft().size() == 0);     // bialy wektor ma byc pusty po ruchu
        BOOST_TEST(stan->getSelectedPiece()==c1);           // sprawdzamy, czy odpowiednia figura zostala wybrana
        BOOST_TEST(stan->getPlayedMove()=="a1");            // sprawdzamy powyzsze zalozenie
        BOOST_TEST(c1->getX() == 1);                // sprawdzamy czy odpowiednie kordynaty zostaly ustawione
        BOOST_TEST(c1->getY() == 1);
        BOOST_TEST(stan->getPieceInformation(9)==0);            // sprawdzamy, czy pole jest puste
        BOOST_TEST(stan->getPieceInformation(0)==4);            // oraz czy czarna krolowa tutaj teraz jest na a1
        c1->promoteIfOnBackRank(stan);
        BOOST_TEST_REQUIRE(stan->getBlack()->getPiecesLeft().size() == 1);    // tylko jedna figura ma byc w wektorze
        BOOST_TEST_REQUIRE(c1 == stan->getBlack()->getPiecesLeft()[0]);     // sprawdzamy, czy c1 to pierwsza i jedyna figura w wektorze nadal
        stan->getBlack()->getPiecesLeft().erase(stan->getBlack()->getPiecesLeft().begin());         // usun c1 z wektora na koniec testow
        BOOST_TEST_REQUIRE(stan->getWhite()->getPiecesLeft().size() == 0);    // i sie upewnij, ze oba wektory sa puste
        BOOST_TEST_REQUIRE(stan->getBlack()->getPiecesLeft().size() == 0);
        stan->changePieceInformation(0,0); // ustawmy krolowa z tablicy currentPosition

        /*
         * Sprawdzmy ostatnie 2 scenariusze - czy prawidlowo wybierana jest figura. W pierwszym przypadku zablokujemy wszystkie figury poza jedna,
         * a w drugim wymusimy bicie
         */
        b1=std::make_shared<Pawn>(1,1,0);
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(2,8,0));        // tak najlatwiej zablokowac figure - poprzez umieszczenie pionka na ostatniej linii
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(6,8,0));
        testWhite->getPiecesLeft().push_back(b1);
        stan->changePieceInformation(57,1); // ustawmy nasze figury w tablicy currentPosition, pionek na b8
        stan->changePieceInformation(61,1); // pionek na f8
        stan->changePieceInformation(0,1); // nasz pionek
        testWhite->makeMove(stan);
        BOOST_TEST_REQUIRE(stan->getSelectedPiece()==b1);           // sprawdzamy, czy odpowiednia figura zostala wybrana
        BOOST_TEST_REQUIRE(stan->getPlayedMove()=="b2");
        stan->setSelectedPiece(nullptr);                            // zmieniamy na nullptr, aby wynik powyzszego testu nie wplywal na wynik ponizszego
        BOOST_TEST_REQUIRE(stan->getSelectedPiece()==nullptr);      // sprawdzamy, czy odpowiednio zostal ustawiony nullptr
        stan->changePieceInformation(9,1); // pionek musial isc na b2
        testBlack->getPiecesLeft().push_back(std::make_shared<Queen>(3,3,1));           // tworzymy krolowa do zbicia
        stan->changePieceInformation(18,4); // i ja ustawiamy w tablicy
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(3,1,0));            // tworzymy losowe pionki, ktore moga sie ruszac
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(5,1,0));
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(7,1,0));
        stan->changePieceInformation(2,1); // i je ustawiamy w tablicy
        stan->changePieceInformation(4,1); // i je ustawiamy w tablicy
        stan->changePieceInformation(6,1); // i je ustawiamy w tablicy
        for (PiecePtr p : stan->getWhite()->getPiecesLeft()) {
            for (int i=0;i<64;i++) p->checkIfThisMoveIsPossible(stan,i);          // aktualizacja pola dla kazdej figury w wektorze
        }
        testWhite->makeMove(stan);                              // symulacja ruchu
        BOOST_TEST_REQUIRE(stan->getSelectedPiece()==b1);           // sprawdzamy, czy odpowiednia figura zostala wybrana
        BOOST_TEST_REQUIRE(stan->getPlayedMove()=="d4");            // i sprawdzamy, czy pole docelowe to d4
    }
BOOST_AUTO_TEST_SUITE_END()