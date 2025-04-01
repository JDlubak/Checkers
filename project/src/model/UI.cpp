#include <iostream>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include "UI.h"
#include "State.h"
#include "Computer.h"
#include "Human.h"
#include "Pawn.h"
#include "typedefs.h"
#include <stdexcept>
/// wypisujemy ładnie sformatowaną aktualną pozycję, oraz jeśli gra się zakończyła, to informację o tym, kto wygrał
void UI::printCurrentPosition(const StatePtr& stan, bool theEnd) {
    system("clear");
    std::cout << "Legenda:\nB - biały pionek, B_K - biała królowa, C - czarny pionek, C_K - czarna królowa\n"
              << "Aktualny stan planszy:\n\n"
              << stan->getBlack()->getPlayerInfo()
              << "\n\n       |       |       |       |       |       |       |       |       \n" << "   8   ";
    for (int i = 55; i >= -8;) {
        /*
         * Ta petla jest dosyc skomplikowana, ale jakos sprobuje wytlumaczyc jej dzialanie:
         * Zaczynamy od wartosci 55. Od razu zwiekszamy ja do 56. Jest to indeks dla pola (x,y)=(1,8)
         * W zaleznosci od tego, jaka jest wartosc w aktualnej pozycji, wyswietl odpowiedni komunikat, lub wyswietl puste pole.
         * W momencie, gdy petla dochodzi do wartosci 62, zwieksza sie do 63 na poczatku i wyswietla informacje o polu (x,y)=(8,8)
         * Wtedy wchodzi do "if". Wytlumaczenie dalsze ponizej w "if".
         */
        i++;
        std::cout << "|";
        switch (stan->getCurrentPosition()[i]) {        // wyswietlamy odpowiedni symbol, w zaleznosci od informacji odczytanej
            case (1):
                std::cout << "   B   ";
                break;
            case (2):
                std::cout << "  B_K  ";
                break;
            case (3):
                std::cout << "   C   ";
                break;
            case (4):
                std::cout << "  C_K  ";
                break;
            default:
                std::cout << "       ";
                break;
        }
        if ((i + 1) % 8 == 0) {
            /*
             * Mamy i=63, wiec (i+1) % 8 = 0 - zostal spelniony warunek. Przechodzimy do nowej linii poprzez odpowiednie komunikaty ponizej
             * Nastepnie jesli i/8 > 0, wyswietlamy i/8 (czyli dla naszego i=63 wyswietlimy 7, dla i=55 wyswietlimy 6 itd.
             * Dlaczzego jesli i/8>0? Dlatego, ze w przeciwnym razie na samym dole, przed A,B,C,D wyswietlilo by sie 0, bo 7/8 = 0 (czesc calkowita)
             * Ostatecznie, zmiejszamy wartosc i o 16. Dla naszego i=63 przejdzie na i=47.
             * Zatem kolejne przejscie petli sie zacznie od i=48 - czyli indeksu pola (x,y)=(1,7)
             * Petla przejdzie w ten sposob po wszystkich polach, az dotrze pola do i=15.
             * Po zmniejszeniu o 16 otrzymamy i=-1, ale przez to, ze zwiekszamy o 1 na poczatku, to dostajemy indeks pola (x,y)=(1,1)
             * Nastepnie, petla idzie do i=7, i wyswietli ostatnie pole (x,y)=(8,1)
             * Tam znowu zajdzie zmniejszenie o 16, wiec i zmniejszy sie do -9 < -8 - petla sie zakonczy
             * Bylo tu troche myslenia, troche metody prob i bledow, ale jakos sie udalo wymyslic ta logike.
             */
            std::cout << std::endl
                      << "_______|_______|_______|_______|_______|_______|_______|_______|_______\n"
                      << "       |       |       |       |       |       |       |       |       \n";
            if (i / 8 > 0) std::cout << "   " << i / 8 << "   ";
            i -= 16;
        }
    }
    std::cout << "       |   A   |   B   |   C   |   D   |   E   |   F   |   G   |   H   \n"
              << "       |       |       |       |       |       |       |       |       \n\n"
              << stan->getWhite()->getPlayerInfo() << std::endl << std::endl;
    if (!theEnd)
        std::cout << "Aktualnie ruch wykonuje: " << (stan->getCurrentColor() ? "Czarny" : "Biały")
                  << std::endl; // w zaleznosci od tego, kto aktualnie gra, wyswietl odpowiedni napis
    else {
        std::cout << "Koniec gry! Wygrana ";
        std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(stan->getBlack());   //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
        std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(stan->getWhite());    //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
        if ((humanBlack && humanBlack->getRemainingTime() == pt::seconds(0)) || (humanWhite && humanWhite->getRemainingTime() == pt::seconds(0)))       // jesli czas dla dowolnego gracza sie skonczyl
             std::cout << (stan->getCurrentColor() ? "czarnego" : "białego") << " (poprzez koniec czasu na zegarze przeciwnika)\n";     // czas sie skonczyl dla gracza, ktory aktualnie nie wykonuje ruchu, dlatego pobieramy informację o tym, kto gra, by wyświetlić komunikat
        else std::cout << (stan->getCurrentColor() ? "białego" : "czarnego") << " (poprzez "    // w przeciwnym razie, w wypadku zablokowania lub zbicia figur, wygrał gracz, który aktualnie nie gra, więc wyświetlamy komunikat na odwrót
                  << ((stan->getWhite()->getPiecesLeft().empty() || stan->getBlack()->getPiecesLeft().empty())      // sprawdzamy czy wektor jakiś jest pusty. jeśli tak - nastąpiło zbicie figur, jeśli nie - nastąpiło zablokowanie
                      ? "zbicie" : "zablokowanie") << " wszystkich figur przeciwnika)\n";       // komunikat
        sleep(3);  // po 5 sekundach wroc do menu glownego - 3 stad i 2 sprzed menu
    }
}
/// wybieramy pole w formacie a1-h8. isTarget decyduje o tym, czy się spytać o pole docelowe, czy pole zajmowane przez figurę.
/// natomiast skipFirstLine pozwala ominąć komunikat o podaniu pola, w sytuacji, gdy nastąpi zły wybór - zastąpione zostanie ono komunikatem błędu.
std::string UI::selectSquare(bool isTarget, bool skipFirstLine) {
    std::string s;
    if (!skipFirstLine) {               // skipFirstLine - w momencie, gdy uzytkownik wpisze nieodpowiednie pole, nie ma co na nowo mu kazac wybrac pola
        if (isTarget) std::cout << "Podaj pole docelowe: ";         // jesli pole jest polem docelowym, to to napisz
        else std::cout << "Wybierz pole, na którym znajduje się twoja figura: ";        // w przeciwnym razie, wybierz pole z twoja figura
    }
    while (true) {
        std::cin >> s;
        s[0] = tolower(s[0]);           // konwersja duzej litery na mala
        if (s.length() == 2 && s[0] >= 97 && s[0] <= 104 && s[1] >= 49 && s[1] <= 56) break;
        // pole sklada sie z 2 skladowych - litery a-h i cyfry 1-8
        // odpowiedni warunek sprawdza, czy nasz string sklada sie z 2 znakow, czy pierwszy znak miesci sie w przedziale <97;104> - w ascii od malego a do malego h
        // i czy drugi znak miesci sie w przedziale <49;56> - w ascii od 1 do 8
        // jesli sie sklada, przerwij petle, w przeciwnym razie wyswietl komunikat o niewlasciwym wpisaniu pola
        badOutput(0);
    }
    return s;
}
/*
 * Metoda używa wujątków, aby wyświetlić komunikat błędu w zależności od wartości argumentu x.
 * Argument x określa, który komunikat błędu ma zostać wyświetlony.
 */
void UI::badOutput(int x) {
    std::string errorString;     // Zmienna przechowująca komunikat błędu
    try {
        switch (x) {
            case (1): errorString = "Na tym polu nie ma figury. Spróbuj ponownie: "; break;
            case (2): errorString = "Wybrano figure przeciwnika. Spróbuj ponownie: "; break;
            case (3): errorString = "Figura nie moze zostac wybrana - nie posiada legalnego ruchu. Spróbuj ponownie: "; break;
            case (4): errorString = "Figura nie moze zostac wybrana - inna figura ma dostępne bicie. Spróbuj ponownie: "; break;
            case (5): errorString = "Podano nielegalny ruch. Spróbuj ponownie: "; break;
            case (6): errorString = "Nazwa pliku nie moze byc pusta. Sprobuj ponownie: "; break;
            case (7): errorString = "Wystąpił nieoczekiwany błąd przy zapisie pliku.\n"; break;
            case (8): errorString = "Wystąpił nieoczekiwany błąd przy otwarciu pliku. Upewnij się, że taki plik istnieje.\n"; break;
            case (9): errorString = "Nie udało się odczytać stanu gry z pliku - informacje są błędne\n"; break;
            default:  errorString = "Bad output! Try again: "; break;
        }
        throw std::invalid_argument(errorString);  // Rzucenie wyjątku z odpowiednim komunikatem błędu
    }
    catch (std::invalid_argument &e) { // Przechwycenie wyjątku
        std::cerr << e.what();  // Wyświetlenie komunikatu błędu na standardowe wyjście błędów
    }
}
/// Ustawiamy informację o graczach w przekazanym stanie - pytamy się, przez kogo ma być sterowany, oraz o nazwę gracza.
void UI::setPlayerInformationUsingUserInput(const StatePtr& stan) {
    /*
     * Funkcja uzywana na poczatku nowej gry - pionki znajda sie w domyslnych pozycjach.
     * Sluzy do ustalania, czy gracz ma byc komputerem, oraz do pobierania nazwy gracza.
     */
    system("clear");                        // czyscimy ekran
    if (!stan->getCurrentColor()) getchar();        // chcemy wylapac znak, by nie wyswietlil sie komunikat bledu - ale tylko za pierwszym razem, gdy ustawiamy bialego
    std::string s;
    bool check;             // do sprawdzenia, czy ustawic gracza, czy komputer
    // pobieramy, kto teraz "gra", by wyswietlic odpowiednio czarny lub bialy
    std::cout << "Czy gracz " << (stan->getCurrentColor() ? "czarny" : "bialy") <<  " ma byc sterowany przez komputer? (t/n): ";
    while (true) {
        std::getline(std::cin, s);      // pobieramy od uzytkownika
        s[0] = tolower(s[0]);               // konwersja na mala litere - umozliwienie wpisania "T"/"N"
        if (s == "t") { check = true; break; } // jesli "t" - ustaw odpowiednia zmienna na prawde i przerwij petle
        else if (s == "n") { check = false; break; } // jesli "n" - ustaw odpowiednia zmienna na falsz i przerwij petle
        else badOutput(0);  // komunikat bledu, jesli nie przerwano petli
    }
    // pobieramy, kto teraz "gra", by wyswietlic odpowiednio czarny lub bialy
    std::cout << "Podaj nazwe gracza "<< (stan->getCurrentColor() ? "czarnego" : "bialego") << " (max 50 znakow): ";
    while (true) {
        std::getline(std::cin, s); // pobierz od uzytkownika
        if (!s.empty() && s.length()<=50) break;  // jesli nazwa gracza ma ponizej 50 znakow, przerwij petle
        // 50 znakow to chyba i tak wystarczajaco jak na nazwe
        badOutput(0);       // komunikat bledu, jesli nie przerwano petli
    }
    std::vector<PiecePtr> pieces;          // uzywane do tworzenia gracza
    if (stan->getCurrentColor()) {      // pobierz aktualny kolor ze stanu - dla czarnego:
        if (check) stan->setBlack(std::make_shared<Computer>(s, stan->getCurrentColor(), pieces));      // jesli wczesniej ustawiono gracza komputerowego, stworz go
        else stan->setBlack(std::make_shared<Human>(s, stan->getCurrentColor(), pieces, pt::minutes(0)));           // w przeciwnym razie, stworz czlowieka. O czas się spytamy za chwilę, więc ustawiamy losową wartość
    }
    else {
        if (check) stan->setWhite(std::make_shared<Computer>(s, stan->getCurrentColor(), pieces));  // jesli wczesniej ustawiono gracza komputerowego, stworz go
        else stan->setWhite(std::make_shared<Human>(s, stan->getCurrentColor(), pieces, pt::minutes(0))); // w przeciwnym razie, stworz czlowieka. O czas się spytamy za chwilę, więc ustawiamy losową wartość
    }
}
/// Zapisujemy grę z przekazanego stanu. Jeśli metoda jest wywołana w ramach testów, nie wyświetlamy komunikatów żadnych, tylko wykorzystujemy przekazaną nazwę pliku.
/// Domyślnie ustalamy, że zmienna przechowująca nazwę pliku jest pusta (wówczas należy ją podać wewnątrz metody), więc nie trzeba jej przekazywać
/// W przypadku normalnej gry, pytamy się gracza o to, czy chce zapisać stan gry, i zapisujemy tylko dla pozytywnej odpowiedzi.
/// Zapisujemy nazwę i informację o tym, kto kontroluje graczy, oraz aktualny kolor i aktualną pozycję - niezbędne informacje do odbudowania stanu gry.
bool UI::saveGame(const StatePtr& stan, bool notTesting, std::string saveFileName) {        // uzywamy notTesting, by nie wyswietlac komunikatow w testach
    /*
     * saveFileName jest parametrem, ktory domyslnie przyjmuje wartosc "" - czyli pusty napis
     * Uzywane jest to do testow - tak by nie wymagaly one podania nazwy pliku od uzytkownika
     */
    std::string s;
    if (notTesting) std::cout << "Chcesz zakończyć grę i zapisać jej aktualny stan do pliku? (t/n): ";
    while (true) {
        if (notTesting) {
            std::getline(std::cin, s);      // pobieramy od uzytkownika
            s[0] = tolower(s[0]);               // konwersja na mala litere - umozliwienie wpisania "T"/"N"
        }
        else s = "t";
        if (s == "t") {
            if (saveFileName.empty()) {
                system("clear");
                std::cout << "Podaj nazwe pliku do zapisu: ";   // ten komunikat chcemy pokazac tylko raz - dlatego przed petla
                // w przeciwnym razie, mozna by zrobic petle while (!saveFileName.empty())
                while (true) {
                    std::getline(std::cin, saveFileName); // pobieramy od uzytkownika nazwe pliku
                    if (!saveFileName.empty()) break;           // nazwa pliku nie moze byc pusta
                    badOutput(6);                            // komunikat o pustym napisie
                }
            }
            std::ofstream plik("../savefiles/"+saveFileName);           // zapisujemy w katalogu savefiles, a nie w cmake-build-debug
            if (plik.is_open()) {
                std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(stan->getBlack());
                std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(stan->getWhite());
                plik << stan->getWhite()->getName() << std::endl
                     << stan->getWhite()->getIsHuman() << std::endl;
                if (humanWhite) plik << humanWhite->getRemainingTime() << std::endl;        // rzutowanie wskaźnika się powiodło - mamy dostęp do czasu, trzeba go zapisać
                plik << stan->getBlack()->getName() << std::endl
                     << stan->getBlack()->getIsHuman() << std::endl;
                if (humanBlack) plik << humanBlack->getRemainingTime() << std::endl;        // rzutowanie wskaźnika się powiodło - mamy dostęp do czasu, trzeba go zapisać
                plik << stan->getCurrentColor() << std::endl;
                for (int i=0;i<64;i++) plik << stan->getPieceInformation(i);
                /*
                 * Do odtworzenia stanu gry ta sa wszystkie informacje, jakich potrzebujemy
                 * W pierwszych 2 linijkach zapisujemy informacje o bialym. Następnie, jeśli jest cżłowiekiem (rzutowanie wskaźnika się powiodło) - zapisujemy jego czas pozostały.
                 * Następnie, w kolejnych 2 lub 3 linijkach zapisujemy te same informacje o czarnym.
                 * Wystaczy nam wiedziec, jak sie nazywaja, i czy sa ludzmi, czy komputerami - kolor wiemy
                 * Kolejna kluczowa informacja jest to, kto teraz gra - co zapisujemy w przedostatniej linijce
                 * I na koncu, zapisujemy aktualna pozycje w postaci 64 cyferek od 0 do 4 - na tej podstawie odtworzymy wektory graczy i ustawimy figury
                 */
                plik.close();  // zamykamy plik
                return true;    // zapis sie powiodl - zwracamy prawde
            } else {
                if (notTesting) badOutput(7); // komunikat bledy
                return false; // nie udalo sie - zwracamy falsz
            }
        }
        else if (s == "n") return false; // jesli "n" - ustaw odpowiednia zmienna na falsz i przerwij petle
        else badOutput(0);  // komunikat bledu, jesli nie przerwano petli
    }
}
/// Wczytujemy grę do przekazanego stanu. Jeśli metoda jest wywołana w ramach testów, nie wyświetlamy komunikatów żadnych, tylko wykorzystujemy przekazaną nazwę pliku.
/// Domyślnie ustalamy, że zmienna przechowująca nazwę pliku jest pusta (wówczas należy ją podać wewnątrz metody), więc nie trzeba jej przekazywać
/// Metoda weryfikuje niezbędne informacje do odbudowania stanu gry. Jeśli nie natrafimy na żaden błąd, zwracamy prawdę - odczyt się udał
bool UI::loadGame(const StatePtr& stan, bool notTesting, std::string saveFileName) { // uzywamy notTesting, by nie wyswietlac komunikatow w testach
    /*
     * Przy odtwarzaniu stanu gry, musimy sie upewnic rowniez, czy plik z zapisem jest poprawny
     * Bedziemy pobierac linijka po linijce i zapisywac w odpowiednich zmiennych
     * A nastepnie weryfikowac poprawnosc zapisu dla kazdego pola
     */
    std::string name, number;       // tutaj beda przechowywane dane
    pt::time_duration duration;     // tutaj zapiszemy czas
    std::vector<PiecePtr> whitePieces, blackPieces;   // pusty wektor, uzywany do stworzenia gracza
    if (saveFileName.empty()) {
        system("clear");
        if (notTesting) std::cout << "Podaj nazwe pliku, z ktorego bedzie prowadzony odczyt: ";   // ten komunikat chcemy pokazac tylko raz - dlatego przed petla
        // w przeciwnym razie, mozna by zrobic petle while (!saveFileName.empty())
        while (true) {
            std::getline(std::cin, saveFileName); // pobieramy od uzytkownika nazwe pliku
            if (!saveFileName.empty()) break;           // nazwa pliku nie moze byc pusta
            badOutput(6);                            // komunikat o pustym napisie
        }
    }
    std::ifstream plik("../savefiles/"+saveFileName);
    bool allGood=true;                  // zalozenie poczatkowe - wszystko poszlo dobrze przy odczycie
    if (plik.is_open()) {
        std::getline(plik, name);                                 // pobieramy 1 linijke z pliku
        if(name.empty() || name.length()>50) allGood = false;           // warunek poprawnosci nazwy - niepusta i mniej niz 50 znakow
        std::getline(plik,number);                                // pobieramy 2 linijke z pliku
        if (number=="0") stan->setWhite(std::make_shared<Computer>(name,0,whitePieces));        // tworzymy bialy komputer, jesli w 2 linijce bylo 0
        else if (number=="1") {
            std::getline(plik,number);                                // pobieramy kolejna linijke z pliku - tutaj musi byc zapisany czas
            try {
                duration = pt::duration_from_string(number);        // konwersja stringa na czas
            } catch (const boost::bad_lexical_cast&) {                 // przechwytujemy wyjątek, jeśli nastąpiła zła konwersja
                allGood=false;      // ustawiamy allGood na false - to pozwoli na końcu pokazać komunikat błędu
            }
            if (allGood) stan->setWhite(std::make_shared<Human>(name,0,whitePieces,duration));   // ustawiamy gracza białego, jeśli odczyt się powiódł do tego momentu
        }
        else allGood=false;             // w innym wypadku, nic nie tworzymy - plik jest niepoprawny
        std::getline(plik, name);                  // pobieramy 3/4 linijke z pliku
        if(name.empty() || name.length()>50) allGood = false;   // warunek poprawnosci nazwy - niepusta i mniej niz 50 znakow
        std::getline(plik,number);                  // pobieramy 4/5 linijke z pliku
        if (number=="0") stan->setBlack(std::make_shared<Computer>(name,1,blackPieces)); // tworzymy czarny komputer, jesli w 4 linijce bylo 0
        else if (number=="1") {
            std::getline(plik,number);                                // pobieramy kolejna linijke z pliku - tutaj musi byc zapisany czas
            try {
                duration = pt::duration_from_string(number);        // konwersja stringa na czas
            } catch (const boost::bad_lexical_cast&) {                 // przechwytujemy wyjątek, jeśli nastąpiła zła konwersja
                allGood=false;      // ustawiamy allGood na false - to pozwoli na końcu pokazać komunikat błędu
            }
            if (allGood) stan->setBlack(std::make_shared<Human>(name,1,blackPieces,duration));   // ustawiamy gracza czarnego, jeśli odczyt się powiódł do tego momentu
        }
        else allGood=false;                                          // w innym wypadku, nic nie tworzymy - plik jest niepoprawny
        std::getline(plik,number);                  // pobieramy kolejną linijke z pliku
        if (number=="0" && allGood) stan->setCurrentColor(false);          // jesli bylo 0 i do tej pory wszystko przebiegło dobrze - teraz gra bialy
        else if (number=="1" && allGood) stan->setCurrentColor(true);     // jesli bylo 1 i do tej pory wszystko przebiegło dobrze - teraz gra czarny
        else allGood=false;             // w innym wypadku, plik jest niepoprawny
        if (allGood) {
            std::getline(plik,
                         name);         // pobieramy ostatnią linijkę z pliku, jeśli do tej pory wszystko było dobrze
            /*
             * Dla aktualnej pozycji mamy 3 warunki poprawnosci:
             * 1 - musimy z pliku miec informacje o wszystkich 64 polach na planszy (i ani jednej informacji wiecej - po co nam to?)
             * 2 - bialych i czarnych figur moze byc maksymalnie po 12
             * 3 - figury musza stac na tej samej przekatnej - wystarczy sprawdzic warunek (x+y)%2=0 dla kazdej figury
             * W przeciwnym razie, z tego pliku nie jestesmy w stanie skutecznie odczytac pozycji
             */
            int countWhite = 0, countBlack = 0;  // maksymalnie mozemy ustawic 12 figur na planszy - wiec bedziemy weryfikowac plik testowy
            if (name.length() == 64) {       // dlugosc napisu musi byc 64 w celu poprawnego odczytu
                for (int i = 0; i < 64; i++) {
                    switch (name[i]) {                  // sprawdzamy znak po znaku
                        case '0':
                            stan->changePieceInformation(i, 0);
                            break;             // ustawiamy puste pole w currentPosition
                        case '1':
                            countWhite++;
                            if (i >= 56) allGood = false;
                            stan->changePieceInformation(i, 1);
                            break; // ustawiamy bialego pionka w currentPosition i zwiekszamy licznik figur. Jesli jego indeks jest wiekszy lub rowny 56 (ostatnia linia) - plik jest nieprawidlowy
                        case '2':
                            countWhite++;
                            stan->changePieceInformation(i, 2);
                            break; // ustawiamy biala krolowa w currentPosition i zwiekszamy licznik figur
                        case '3':
                            countBlack++;
                            if (i <= 7) allGood = false;
                            stan->changePieceInformation(i, 3);
                            break; // ustawiamy czarnego pionka w currentPosition i zwiekszamy licznik figur. Jesli jego indeks jest mniejszy lub rowny 7 (ostatnia linia) - plik jest nieprawidlowy
                        case '4':
                            countBlack++;
                            stan->changePieceInformation(i, 4);
                            break; // ustawiamy czarna krolowa w currentPosition i zwiekszamy licznik figur
                        default:
                            allGood = false;
                            break;                              // dziwny znak - cos poszlo nie tak, nie mozna odczytac z tego pliku
                    }
                    if (name[i] != '0') {
                        if ((((i + 8) / 8) + ((i % 8) + 1)) % 2 != 0) allGood = false;
                        /*
                         * jesli znak to nie zero, to znaczy, ze jest tam figura (lub cos innego, ale to odrzuci switch case)
                         * Dlatego, konwertujemy i na x i y, i dodajemy do siebie (y=i+8/8, x=i%8+1)
                         * Nastepnie, sprawdzamy, czy jestesmy na odpowiedniej przekatnej - czyli czy suma x i y jest podzielna przez 2. Jesli nie jest - plik jest niepoprawmy
                         */
                    }
                }
            } else allGood = false;         // jesli odczytalismy mniej lub wiecej niz 64 znaki, plik jest niepoprawny
            if (countWhite > 12 || countBlack > 12 || countWhite == 0 || countBlack == 0)
                allGood = false;              // jesli ktorys figur bylo za duzo, albo ktorys figur nie ma, to nie mozna odczytac z pliku
        }
        if (!plik.eof()) allGood = false; // ostatni warunek - teraz jestesmy na koncu pliku
        if (!allGood && notTesting) badOutput(9);         // informujemy, ze cos jest nie tak z tym plikiem
    } else {
        if (notTesting) badOutput(8); // komunikat bledu
        return false;           // nie mozna otworzyc pliku - wiec nie udalo sie odczytac stanu
    }
    return allGood;         // zwroc informacje o tym, czy plik byl poprawny
}
/// menu główne naszej gry. pytamy się użytkownika, co chce zrobić, w zależności od odpowiedzi, wywołujemy odpowiednie metody.
void UI::mainMenu() {
    StatePtr stan = std::make_shared<State>(); // deklaracja stanu
    system("clear");
    std::cout << "Witamy w grze w warcaby.\nAutorzy - Jakub Dłubak, Radosław Popowicz.\nPress enter to continue..."; // przywitanie
    getchar(); // za kolejnym uruchomieniem gry nie chcemy witac uzytkownika, ale musimy mu dac mozliwosc przeczytania powyzszego komunikatu
    std::string wybor = "";
    while (wybor != "4") {
        sleep(2); // dajemy czas na przeczytanie ewentualnego komunikatu bledu
        system("clear"); // bo zaraz wyczyscimy ekran
        std::cout << "Twoje dostępne opcje to:\n" // informacja dla uzytkownika
                  << "1 - Zapoznaj się z zasadami gry i z działaniem programu,\n"
                  << "2 - Zacznij nową grę - od domyślnego stanu gry,\n"
                  << "3 - Zacznij grę ze stanu załadowanego z pliku,\n"
                  << "4 - Opuść program.\n"
                  << "Twój wybór to: ";
        std::cin >> wybor;
        if (wybor == "1" || wybor == "2" || wybor == "3" || wybor == "4") {
            switch (wybor[0]) {
                case '1': printGameRules(); break;
                case '2': stan->prepareInitialState(false); stan->playGame(); break;
                case '3': getchar(); if (loadGame(stan, true)) { stan->prepareInitialState(true); ungetc('X', stdin); stan->playGame(); } break;
                // ungetc umieszcza losowy znak w buforze stdin, który zostanie wyłapany przez getchar w playGame - w przeciwnym razie trzeba czekać na enter po uruchomieniu gry z pliku przed 1 ruchem.
                case '4': std::cout << "Do widzenia!\n"; break;
            }
        } else badOutput(0);
    }
}
/// wypisanie zasad i dzialania programu
void UI::printGameRules() {
    system("clear");
    std::cout << "Gra w warcaby przeznaczona jest dla 2 graczy - ludzi lub komputerów.\n"
              << "Celem gry jest uniemożliwienie poruszania się twojemu przeciwnikowi. Można to zrobić na 2 sposoby:\n"
              << "1 - zbicie wszystkich figur przeciwnika,\n"
              << "2 - zablokowanie wszystkich figur przeciwnika.\n"
              << "Istnieją 2 rodzaje figur - pionek i królowa.\n"
              << "Pionek może się poruszać o 1 pole do przodu po przekątnej, a królowa o dowolną ilość pól po przekątnej do przodu i do tyłu.\n"
              << "Na drodze twoich figur nie mogą stanąć inne twoje figury - taki ruch będzie nielegalny. Natomiast jeśli natrafisz na figurę przeciwnika, to możesz ją zbić.\n"
              << "Pionek może zbijać zarówno do przodu, jak i do tyłu każdą figurę przeciwnika, która znajdzie się w odległości 1 pole po przekątnej.\n"
              << "Po zbiciu, pionek kończy swój ruch 1 pole po przekątnej za zbitą figurą, a ta jest usuwana z planszy.\n"
              << "Natomiast królowa może zbijać figurę znajdującą się dowolną ilość pól po przekątnej w dowolną stronę, o ile na jej drodze nie stanie własna figura, pole docelowe jest puste, a po drodze przeskoczy nad tylko 1 figurą przeciwnika.\n"
              << "Na starcie każdego twojego ruchu, program poprosi cię o wpisanie pola, na którym się znajduję twoja figura.\n"
              << "Poprawny format pola składa się z litery a-h, oraz cyfry 1-8, np. A3, b7, g4.\n"
              << "Należy pamiętać przy wyborze o 3 zasadach - figura musi należeć do ciebie, musi mieć możliwość poruszania się, oraz o fakcie, że bicie jest przymusowe.\n"
              << "Wybór figury, która nie ma dostępnego bicia, w sytuacji, gdy inna figura ma możliwość bicia, zakończy się niepowodzeniem.\n"
              << "Po wyborze figury, przechodzimy do wpisania pola docelowego, w takim samym formacie jak wcześniej. Jeśli ruch jest nielegalny, nie będzie możliwości wykonania go.\n"
              << "Jeśli się uda wykonać poprawny ruch, stan planszy jest aktualizowany, i twój przeciwnik wykonuje ruch.\n"
              << "Jest jednak wyjątek od powyższej reguły. Jeśli w twoim ruchu zbijasz figurę przeciwnika, po czym masz możliwość ponownego bicia, musisz zbić kolejną figurę.\n"
              << "Wówczas program poprosi cię o wpisanie pola docelowego jeszcze raz, i tak w kółko, dopóki będą dostępne bicia.\n"
              << "Miej na uwadze fakt, że figura przemieści się wizualnie na planszy dopiero po ostatnim wykonanym biciu - czyli po ostatnim wpisanym polu.\n"
              << "Press enter to continue...\n";
    getchar(); getchar();                   // Podwojny getchar - bo inaczej nie dziala zgodnie z zamierzeniem, prawdopodobnie pierwszy pobiera enter po wpisaniu "1" w menu
}
/// ustawianie pierwotnego czasu dla każdego z ludzi
void UI::selectGameTime(const StatePtr &stan) {
    system("clear");        // czyścimy ekran
    std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(stan->getWhite());    //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
    std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(stan->getBlack());   //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
    if (!humanWhite || !humanBlack)     // to oznacza, ze jeden z graczy jest komputerem, wiec wyswietlamy poniższy komunikat
        std::cout << "Gracza komputerowego czas nie obowiązuje - i tak będzie wykonywał ruchy natychmiastowo.\n";
    std::cout << "Wybierz czas gry na gracza w minutach (liczba naturalna, od 1 do 59):\n";     // komunikat
    int x;      // tu zapiszemy nasz czas
    while (true) {
        if (std::cin>>x && x>=1 && x<=59) break;        // zapisujemy do inta, wiec musimy miec pewnosc o poprawnosci
        else {
            badOutput(0);               // komunikat bledu
            std::cin.clear();               // wyczyszczenie flag błędów
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // oczyszczenie całego strumienia
        }
    }
    if (humanWhite) {       // jeśli rzutowanie wskaźnika się powiodło, to możemy ustawić czas
        humanWhite->setRemainingTime(pt::minutes(x));
    }
    if (humanBlack) {      // jeśli rzutowanie wskaźnika się powiodło, to możemy ustawić czas
        humanBlack->setRemainingTime(pt::minutes(x));
    }
}

