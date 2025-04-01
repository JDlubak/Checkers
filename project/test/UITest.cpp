#include <boost/test/unit_test.hpp>
#include "State.h"
#include "UI.h"
#include "Player.h"
/*
 * W zasadzie jedyma rzecz do przetestowania tutaj to zapis i odczyt z pliku. W tym celu wykorzystamy przykladowy dobry stan i przykladowy zly stan.
 * Pozostale metody wymagaja komunikacji z uzytkownikiem, wiec nie bylyby przeprowadzane automatycznie
 */

struct TestSuiteUIFixture {
    StatePtr stan = std::make_shared<State>();
    StatePtr stan2 = std::make_shared<State>();
    const int testCurrentPosition[64] = {     // Zapiszemy ta pozycje do pliku testSave i potem sprobujemy odczytac
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0,
            0, 0, 3, 0, 3, 0, 0, 0,
            0, 1, 0, 4, 0, 2, 0, 3,
            1, 0, 2, 0, 0, 0, 1, 0,
            0, 3, 0, 0, 0, 0, 0, 0
    };
    std::string positionString = "";  // tutaj bedziemy kopiować znaki po odczytaniu
    TestSuiteUIFixture() {}
    ~TestSuiteUIFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(TestSuite, TestSuiteUIFixture)

    BOOST_AUTO_TEST_CASE(UIAssertionsTests) {

    BOOST_TEST_REQUIRE(!UI::loadGame(stan,0,"Example Corrupt Save File"));          // corrupt to nie mozemy
    BOOST_TEST_REQUIRE(UI::loadGame(stan,0,"Example Good Save File"));              // good to mozemy
    BOOST_TEST_REQUIRE(stan->getWhite()->getPlayerInfo()=="Biały BOT: Checker Bot v0.14");      // tak sprawdzamy poprawnosc ustawienia gracza bialego
    BOOST_TEST_REQUIRE(stan->getBlack()->getPlayerInfo()=="Czarny Gracz: Random Name, pozostały czas: 00:03:02");         // tutaj to samo dla czarnego
    BOOST_TEST_REQUIRE(stan->getCurrentColor());                // plik mowi, ze gra czarny
    for (int i=0;i<64;i++) positionString+=std::to_string(stan->getPieceInformation(i));
    BOOST_TEST_REQUIRE(positionString=="0040001001000101000010300303000010003010030103031030301003030202");     // taki ciąg mieliśmy w pliku
    BOOST_TEST(stan->getPieceInformation(2)==4);        // losowo sprawdzamy 4 pozycje z currentPosition
    BOOST_TEST(stan->getPieceInformation(63)==2);
    BOOST_TEST(stan->getPieceInformation(6)==1);
    BOOST_TEST(stan->getPieceInformation(20)==1);
    /*
     * Teraz zmienimy dane, zapiszemy do pliku, a potem spróbujemy skopiować do drugiego stanu
     */
    stan->getWhite()->setName("new White");
    stan->getBlack()->setName("new Black");
    stan->setCurrentColor(false);
    stan->setCurrentPosition(testCurrentPosition);
    BOOST_TEST_REQUIRE(UI::saveGame(stan,0,"testFile"));            // musimy zapisać do pliku testFile
    BOOST_TEST_REQUIRE(UI::loadGame(stan2,0,"testFile"));       // oraz odczytać z niego, by iść dalej
    BOOST_TEST(stan2->getWhite()->getPlayerInfo()=="Biały BOT: new White");      // sprawdzamy poprawnosc ustawienia gracza bialego
    BOOST_TEST(stan2->getBlack()->getPlayerInfo()=="Czarny Gracz: new Black, pozostały czas: 00:03:02");         // tutaj to samo dla czarnego
    BOOST_TEST(!stan2->getCurrentColor());                // plik mowi, ze gra bialy
    BOOST_TEST(std::equal(stan2->getCurrentPosition(), stan2->getCurrentPosition() + 64, testCurrentPosition)); // porownujemy pozycje
    }


BOOST_AUTO_TEST_SUITE_END()