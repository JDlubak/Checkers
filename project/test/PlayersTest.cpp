#include <boost/test/unit_test.hpp>
#include "typedefs.h"
#include "Human.h"
#include "Pawn.h"
#include "Computer.h"
#include "State.h"

/*
 * Metoda makeMove odgrywala znaczaca role w testach figur, wiec tutaj juz nie bedzie testowana
 * Natomiast metoda moveVerification jest wykorzystywana do powyzszej metody - wiec tez jej testy zostaly dokonane
 * Z racji, ze makeMove dla czlowieka i dla komputera ustala podobne ograniczenia wyboru figury, mozna ustalic, ze testy zostaly pokryte tez dla tej metody
 * Glowny problem polega na tym, ze nie da sie wykonac tych testow automatycznie dla czlowieka, poniewaz korzysta on z interfejsu uzytkownika, wiec testy nie bylyby automatyczne
 */

struct TestSuitePlayersFixture {
    std::vector<PiecePtr> whitePieces;
    std::vector<PiecePtr> blackPieces;
    PlayerPtr testWhite = std::make_shared<Human>("humanPlayer", 0, whitePieces,pt::minutes(5));
    PlayerPtr testBlack = std::make_shared<Computer>("computerPlayer", 1, blackPieces);
    TestSuitePlayersFixture() {}
    ~TestSuitePlayersFixture() {}
    std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(testWhite);    //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
    std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(testBlack);    //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu

};

BOOST_FIXTURE_TEST_SUITE(TestSuite, TestSuitePlayersFixture)

    BOOST_AUTO_TEST_CASE(PlayersAssertionsTests) {
        BOOST_TEST_REQUIRE(humanWhite);         // to rzutowanie musi się powieść
        BOOST_TEST_REQUIRE(!humanBlack);        // a to nie
        /*
         * Teraz poprawność pól za pomocą getterów
         */
        BOOST_TEST(testWhite->getName()=="humanPlayer");
        BOOST_TEST(testBlack->getName()=="computerPlayer");
        BOOST_TEST(!testWhite->getPlayedColor());
        BOOST_TEST(testBlack->getPlayedColor());
        BOOST_TEST(testWhite->getPiecesLeft()==whitePieces);
        BOOST_TEST(testBlack->getPiecesLeft()==blackPieces);
        BOOST_TEST(testWhite->getIsHuman());
        BOOST_TEST(!testBlack->getIsHuman());
        BOOST_TEST(testWhite->getPlayerInfo()=="Biały Gracz: humanPlayer, pozostały czas: 00:05:00");
        BOOST_TEST(testBlack->getPlayerInfo()=="Czarny BOT: computerPlayer");
        BOOST_TEST(humanWhite->getRemainingTime()==pt::minutes(5));
        /*
         * Ustawiamy wszystko na odwrot by przetestowac settery.
         * Wiadomo, raczej w trakcie gry raczej sie tych pol nie zmienia, ale wazne to jest w kontekscie wczytywania informacji z pliku
         * Nie bedziemy zmieniać tylko informacji o tym, czy gracz jest człowiekiem, czy komputerem - bo pomiesza to metodę getPlayerInfo
         */
        testWhite->setName("humanPlayerChanged");
        testBlack->setName("computerPlayerChanged");
        testWhite->setPlayedColor(1);
        testBlack->setPlayedColor(0);
        testWhite->setPiecesLeft(blackPieces);
        testBlack->setPiecesLeft(whitePieces);
        humanWhite->setRemainingTime(pt::seconds(78));
        BOOST_TEST(testWhite->getName()=="humanPlayerChanged");
        BOOST_TEST(testBlack->getName()=="computerPlayerChanged");
        BOOST_TEST(testWhite->getPlayedColor());
        BOOST_TEST(!testBlack->getPlayedColor());
        BOOST_TEST(testWhite->getPiecesLeft()==blackPieces);
        BOOST_TEST(testBlack->getPiecesLeft()==whitePieces);
        BOOST_TEST(testWhite->getPlayerInfo()=="Czarny Gracz: humanPlayerChanged, pozostały czas: 00:01:18");
        BOOST_TEST(testBlack->getPlayerInfo()=="Biały BOT: computerPlayerChanged");
        BOOST_TEST(humanWhite->getRemainingTime()==pt::seconds(78));

        // Ustawianie wartosci z powrotem na "normalne"
        testWhite->setPlayedColor(0);
        testBlack->setPlayedColor(1);
        testWhite->setPiecesLeft(whitePieces);
        testBlack->setPiecesLeft(blackPieces);
        /*
         * Ostatnia rzecza do przetestowania jest szukanie figury, zatem:
         */
        PiecePtr searchedPiece;                            // tutaj bedzie zwracana nasza figura poszukiwana
        BOOST_TEST(testWhite->getPiecesLeft().empty());         // sprawdzmy, czy wektory sa puste
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(1,1,0));        // przykladowe figury
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(1,2,0));
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(2,1,0));
        testWhite->getPiecesLeft().push_back(std::make_shared<Pawn>(2,2,0));
        BOOST_TEST(testWhite->getPiecesLeft().size()==4);           // sprawdzamy czy rozmiar wektora to 4
        searchedPiece=testWhite->findPiece(testWhite->getPiecesLeft(),2,1); // szukamy figury o x=2 i y=1
        BOOST_TEST_REQUIRE(searchedPiece->getX()==2);           // sprawdzamy x
        BOOST_TEST_REQUIRE(searchedPiece->getY()==1);           // sprawdzamy y
        testBlack->getPiecesLeft().push_back(std::make_shared<Pawn>(3,4,1));        // dodajemy figure o szukanych kordynatach (ponizej) do wektora czarnego
        searchedPiece=testWhite->findPiece(testWhite->getPiecesLeft(),3,4);     // tego nie znajdziemy
        BOOST_TEST_REQUIRE(searchedPiece== nullptr);            // wiec musi byc nullptr - sprawdzamy
        searchedPiece=testBlack->findPiece(testBlack->getPiecesLeft(),3,4);     // u czarnego powinnismy znalezc ta figure
        BOOST_TEST_REQUIRE(searchedPiece->getX()==3);           // wiec sprawdzamy x i y zeby sie upewnic
        BOOST_TEST_REQUIRE(searchedPiece->getY()==4);
    }

BOOST_AUTO_TEST_SUITE_END()