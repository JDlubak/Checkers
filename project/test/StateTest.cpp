#include <boost/test/unit_test.hpp>
#include "typedefs.h"
#include "Pawn.h"
#include "Queen.h"
#include "Computer.h"
#include "State.h"

/*
 * Czesc metod zostala juz przetestowana wraz z testowaniem ruchow figur - tutaj beda sprawdzone pozostale metody
 */

struct TestSuiteStateFixture {
    PiecePtr testPiece;
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
    TestSuiteStateFixture() {}
    ~TestSuiteStateFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TestSuite, TestSuiteStateFixture)

    BOOST_AUTO_TEST_CASE(StateAssertionsTests) {
    // na poczatku test konstruktora za pomoca getterow
    BOOST_TEST(!stan->getCurrentColor());
    BOOST_TEST(std::equal(stan->getCurrentPosition(), stan->getCurrentPosition() + 64, testCurrentPosition));
    BOOST_TEST(stan->getWhite()==testWhite);
    BOOST_TEST(stan->getBlack()==testBlack);
    BOOST_TEST(stan->getSelectedPieceString()=="");             // nie bylo to ustawiane przy konstruktorze - puste pole
    BOOST_TEST(stan->getPlayedMove()=="");                      // nie bylo to ustawiane przy konstruktorze - puste pole
    BOOST_TEST(stan->getSelectedPiece()== nullptr);
    // ustawiamy nowe wartosci za pomoca setterow, ktore testujemy
    stan->setSelectedPiece(testPiece);      // losowa figura
    stan->setSelectedPieceString("d4");     // losowe pole
    stan->setPlayedMove("e5");             // losowy ruch
    stan->setBlack(testWhite);          // tymczasowo na odwrot ustawiamy graczy
    stan->setWhite(testBlack);
    int newPosition[64];        // przykladowa nowa pozycja
    for (int i=0;i<64;i++) newPosition[i]=0;
    stan->setCurrentPosition(newPosition);
    stan->setCurrentColor(!stan->getCurrentColor());               // przeciwny kolor
    // i za pomoca getterow weryfikujemy nasze settery
    BOOST_TEST(stan->getCurrentColor());
    BOOST_TEST(std::equal(stan->getCurrentPosition(), stan->getCurrentPosition() + 64, newPosition));
    BOOST_TEST(stan->getWhite()==testBlack);
    BOOST_TEST(stan->getBlack()==testWhite);
    BOOST_TEST(stan->getSelectedPieceString()=="d4");             // nie bylo to ustawiane przy konstruktorze - puste pole
    BOOST_TEST(stan->getPlayedMove()=="e5");                      // nie bylo to ustawiane przy konstruktorze - puste pole
    BOOST_TEST(stan->getSelectedPiece()== testPiece);
    /*
     * Teraz przejdzmy do przetestowania kilku uzytecznych metod, zaczynajac od jednej z najwazniejszych metod:
     */
    BOOST_TEST_REQUIRE(stan->convertStringToArrayIndex(stan->getSelectedPieceString())==27);      // d4 ma indeks 27
    BOOST_TEST_REQUIRE(stan->convertStringToArrayIndex(stan->getPlayedMove())==36);                   // e5 ma indeks 36
    BOOST_TEST_REQUIRE(stan->convertStringToArrayIndex("i9")>63);               // pole powyzej w skali - w praktyce go nie mozemy ustawic
    BOOST_TEST_REQUIRE(stan->convertStringToArrayIndex("a0")<0);                // pole ponizej skali - w praktyce nie mozemy go ustawic
    BOOST_TEST(stan->getPieceInformation(3)==0);              // domyslnie - puste pole
    stan->changePieceInformation(3,3);          // ustawiamy czarnego pionka na d1
    BOOST_TEST(stan->getPieceInformation(3)==3);             // sprawdzamy, czy sie udalo
    stan->changePieceInformation(3,5);          // takiej wartosci nie mozemy ustawic - od 0 do 4
    BOOST_TEST(stan->getPieceInformation(3)==3);            // sprawdzamy, czy dalej pionek stoi na d1
    BOOST_TEST_REQUIRE(stan->getPieceInformation(64)==0);           // wyszlismy poza skale indeksu - musi zwrocic 0
    stan->changePieceInformation(3,0);          // usuwamy tego pionka
    BOOST_TEST(!stan->decideIfGameShouldContinue());                // obydwa wektory sa puste - gra sie nie moze odbyc
    stan->getWhite()->getPiecesLeft().push_back(std::make_shared<Pawn>(2,8,0)); // pionka na linii 8 sie nie powinno tworzyc, ale to jest po to, by szybko zablokowac figure bialego
    stan->changePieceInformation(57,1);             // ustawiamy naszego pionka w tablicy currentPosition na polu b8
    BOOST_TEST(!stan->decideIfGameShouldContinue());                // tylko bialy ma teraz pionki - gra dalej nie moze sie odbyc
    stan->getBlack()->getPiecesLeft().push_back(std::make_shared<Pawn>(2,4,1));         // tworzymy pionka czarnego, ktory moze sie ruszac
    stan->changePieceInformation(25,1);             // ustawiamy naszego pionka w tablicy currentPosition na polu b4
    BOOST_TEST(stan->decideIfGameShouldContinue());                // obydwa kolory maja po figurze, gra czarny, ktory ma ruch - gra moze sie odbyc
    stan->setCurrentColor(0);                           // teraz bialy niech zagra
    BOOST_TEST(!stan->decideIfGameShouldContinue());                // pionek bialego nie moze sie ruszac - zakoncz gre


    }

BOOST_AUTO_TEST_SUITE_END()