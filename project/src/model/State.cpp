#include <unistd.h>
#include <utility>
#include "State.h"
#include "Piece.h"
#include "Player.h"
#include "UI.h"
#include "Pawn.h"
#include "Queen.h"
#include "Human.h"

/// konstruktor bezparametrowy dodatkowy
State::State() = default;
/// konstruktor parametrowy, ustawiający kluczowe pole
State::State(bool currentColor, const int *currentPosition, PlayerPtr white, PlayerPtr black) : currentColor(currentColor),
                                                 white(std::move(white)), black(std::move(black)) {
    for (int i=0;i<64;i++) {
        State::currentPosition[i]=currentPosition[i];
    }
    selectedPiece=nullptr;
    // nie ma sensu ustawiac selectedPieceString i playedMove, bo to ustawia gracz/komputer przy kazdym ruchu
}
/// getter koloru, który aktualnie gra
bool State::getCurrentColor() const {
    return currentColor;
}
/// setter koloru, który aktualnie gra
void State::setCurrentColor(bool newCurrentColor) {
    State::currentColor = newCurrentColor;
}
/// getter aktualnej pozycji na planszy
const int *State::getCurrentPosition() const {
    return currentPosition;
}
/// setter aktualnej pozycji na planszy
void State::setCurrentPosition(const int *newCurrentPosition) {
    for (int i=0;i<64;i++) {
        State::currentPosition[i]=newCurrentPosition[i];
    }
}
/// getter białego
PlayerPtr State::getWhite() const {
    return white;
}
/// setter białego
void State::setWhite(PlayerPtr newWhite) {
    State::white=std::move(newWhite);
}
/// getter czarnego
PlayerPtr State::getBlack() const {
    return black;
}
/// setter czarnego
void State::setBlack(PlayerPtr newBlack) {
    State::black=std::move(newBlack);
}
/// getter pola docelowego - w formacie a1-h8
const std::string &State::getPlayedMove() const {
    return playedMove;
}
/// setter pola docelowego - w formacie a1-h8
void State::setPlayedMove(const std::string &newPlayedMove) {
    State::playedMove = newPlayedMove;
}
/// getter pola wybranej figury - w formacie a1-h8
const std::string &State::getSelectedPieceString() const {
    return selectedPieceString;
}
/// setter pola wybranej figury - w formacie a1-h8
void State::setSelectedPieceString(const std::string &newSelectedPieceString) {
    State::selectedPieceString = newSelectedPieceString;
}
/// setter wybranej figury
void State::setSelectedPiece(PiecePtr newSelectedPiece) {
    State::selectedPiece = std::move(newSelectedPiece);
}
/// getter wybranej figury
PiecePtr State::getSelectedPiece() const {
    return selectedPiece;
}
/*
 * Przygotowanie początkowego stanu
 * Jeśli nie załadowano z pliku, wowołujemy metody do ustawiania informacji o graczach, oraz ustawiamy figury w tablicy currentPosition na domyślnych pozycjach
 * Następnie, zarówno dla domyślnej pozycji, jak i tej wczytanej z pliku, odwzorowujemy sytuację z currentPosition w wektorach graczy
 */
void State::prepareInitialState(bool isLoadedFromFile) {
    if (!isLoadedFromFile) {  // jesli wczytalismy z pliku, nie ma co ustalac domyslnej pozycji
        currentColor = false;          // metoda setPlayerInformationUsingUserInput odczytuje ze stanu aktualny kolor - wiec ustaw kolor na bialy
        UI::setPlayerInformationUsingUserInput(shared_from_this());        // ustawianie bialego
        setCurrentColor(
                !getCurrentColor());                     // metoda setPlayerInformationUsingUserInput odczytuje ze stanu aktualny kolor - wiec zmien kolor na czarny
        UI::setPlayerInformationUsingUserInput(shared_from_this());        // ustawianie bialego
        setCurrentColor(!getCurrentColor());                     // zaczyna bialy
        if (getWhite()->getIsHuman() || getBlack()->getIsHuman()) UI::selectGameTime(shared_from_this());
        for (int & i : currentPosition) i = 0;     // poczatkowo kazde pole jest puste
        for (int i = 0; i < 23; i += 2) {       // 0 - pole a1, 23 - pole h3 (a ostatniego pionka ustawiamy na g3 - polu 22)
            currentPosition[i] = 1;     // 1 - pionek bialy
            if (i == 6) i = 7;      // przejscie z 1 linii do 2 - po zakonczeniu petli doda sie jeszcze 2 i bedzie i=9 - pole b2
            if (i == 15) i = 14;    // przejscie z 2 linii do 3 - po zakonczeniu petli doda sie jecze 2 i bedzie i=16 - pole a3
        }
        for (int i = 63; i > 40; i -= 2) {  // 63 - pole h8, 40 - pole a6 (a ostatniego pionka ustawiamy na b6 - polu 41)
            currentPosition[i] = 3;     // 3- pionek czarny
            if (i == 57) i = 56;        // przejscie z 8 linii do 7 - po zakonczeniu petli odejmie sie jeszcze 2 i bedzie i=54 - pole g7
            if (i == 48) i = 49;        // przejscie z 7 linii do 6 - po zakonczeniu petli odejmie sie jeszcze 2 i bedzie i=47 - pole h6
        }
    }
    for (int i = 0; i < 64; i++) {          // dla calej currentPosition
        switch (getPieceInformation(i)) {   // pobieramy informacje o kazdym polu i dodajemy do wektorow odpowiednie figury
            //(i%8)+1 - konwersja na x, (i+8)/8 - konwersja na y
            case 1: getWhite()->getPiecesLeft().push_back(std::make_shared<Pawn>(((i % 8) + 1),((i + 8) / 8),0)); break;        // bialy pionek
            case 2: getWhite()->getPiecesLeft().push_back(std::make_shared<Queen>(((i % 8) + 1),((i + 8) / 8),0)); break;   // biala krolowa
            case 3: getBlack()->getPiecesLeft().push_back(std::make_shared<Pawn>(((i % 8) + 1),((i + 8) / 8),1)); break;    // czarny pionek
            case 4: getBlack()->getPiecesLeft().push_back(std::make_shared<Queen>(((i % 8) + 1),((i + 8) / 8),1)); break; //czarna krolowa
        }
    }
}
/// konwersja pola z formatu a1-h8 na liczbę 0-63, wykonywaną w celach obliczeniowych dla innych metod
int State::convertStringToArrayIndex(std::string testString) {
    return 8*(testString[1]-'1')+testString[0]-'a';     // "a" w ascii - 97, "1" w ascii - 49
}
/// pobranie pojedynczej informacji z currentPosition, o zdefiniowanym indeksie
int State::getPieceInformation(int index) const {
    if (index>=0 && index<=63) return currentPosition[index];
    else return 0;      // raczej nie ma szans wyjsc za zakres, ale jesli sie tak stalo, zwroc 0 - puste pole
}
/// zmiana pojedynczej informacji w currentPosition, o zdefiniowanym indeksie
void State::changePieceInformation(int index, int newInformation) {
    if (index>=0 && index<=63 && newInformation>=0 && newInformation<=4) State::currentPosition[index]=newInformation; // kluczowe indeksy to 0-63, a informacje to 0-4
}
/// Jedna z głównych metod programu, służąca do grania w grę
/// Jeśli gra powinna być kontynuowana (wartość zwrócona przez inną metodę), to pokazujemy aktualną pozycję za pomocą metody z UI.
/// Następnie, w zależności od aktualnego koloru, gramy w warcaby. Na początku ruchu gracza ludzkiego pytamy się o to, czy chce zapisać stan gry do pliku.
/// Jeśli tak, to kończymy grę, a jeśli nie, wywołujemy metodę gracza do wykonania ruchu. Tutaj też znajduje się wywołanie metody do promocji figury po zakończonym ruchu.
/// (Tylko w warcabach rosyjskich figura może wypromować, a potem jeszcze bić w tym samym ruchu, i pomimo faktu, że to jest jedyna różnica między tymi warcabami a rosyjskimi,
/// to w przeważającej większości wersji figura promuje dopiero w sytuacji, gdy zakończy swój ruch na ostatniej linii, co zostało odwzorowane w grze)
/// Następnie, zmieniamy kolor aktualny na przeciwny, i dopóki gra powinna być kontynuowana, gramy.
void State::playGame() {
    bool gameSaved=false;
    while(decideIfGameShouldContinue()) {           // graj dopoki obydwa wektory nie sa puste
        UI::printCurrentPosition(shared_from_this(), false);          // pokaz aktualna pozycje
        pt::ptime startTime = pt::second_clock::local_time();           // zaczynamy mierzenie czasu dla aktualnego ruchu
        if (getCurrentColor()) {
            if (getBlack()->getIsHuman()) {       // komputera sie nie bedziemy pytac o zapis, bo nie odpowie i tak
                getchar();          // trzeba złapać jakiś znak, inaczej od razu wyświetla badOutput z save'a
                if (UI::saveGame(shared_from_this(),true)) {
                    gameSaved = true;
                    break;
                }  // spytaj sie o zapis gry, jesli zapisano, to ustaw to, i przerwij petle
            }
            getBlack()->makeMove(shared_from_this());       // wykonaj ruch
        } else {
            if (getWhite()->getIsHuman()) { // komputera sie nie bedziemy pytac o zapis, bo nie odpowie i tak
                getchar();          // trzeba złapać jakiś znak, inaczej od razu wyświetla badOutput z save'a
                if (UI::saveGame(shared_from_this(),true)) {
                    gameSaved = true;
                    break;    // spytaj sie o zapis gry, jesli zapisano, to ustaw to, i przerwij petle
                }
            }
            getWhite()->makeMove(shared_from_this());    // wykonaj ruch
        }
        getSelectedPiece()->promoteIfOnBackRank(shared_from_this());          // wywolaj metode promoteIfOnBackRank, ktora sprawdzi, czy figura jest na ostatniej linii, i jak tak, to ja wypromuje
        pt::ptime endTime = pt::second_clock::local_time();           // kończymy mierzenie czasu
        if (getCurrentColor()) {
            std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(getBlack());  //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
            if (humanBlack) {
                humanBlack->setRemainingTime(humanBlack->getRemainingTime()-pt::time_duration(endTime-startTime));      // ustawiamy nowy czas - od czasu pierwotnego odejmujemy roznice miedzy czasem koncowym a poczatowym danego ruchu
            }
        }
        else {
            std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(getWhite());  //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
            if (humanWhite) {
                humanWhite->setRemainingTime(humanWhite->getRemainingTime()-pt::time_duration(endTime-startTime));    // ustawiamy nowy czas - od czasu pierwotnego odejmujemy roznice miedzy czasem koncowym a poczatowym danego ruchu
            }
        }
        setCurrentColor(!getCurrentColor());    // zmien kolor na przeciwny
        sleep(1);      // drobne opoznienie miedzy ruchami - bo inaczej gra 2 komputerow sie konczy natychmiastowo
        }
    if (!gameSaved) UI::printCurrentPosition(shared_from_this(),true);     // jesli gra nie jest zapisana, wyswietl koncowa pozycje z informacja o tym kto wygral
}
/// Sprawdzamy, czy gra powinna być kontynuowana na początku ruchu
/// Na początku sprawdzamy zegarki graczy - jeśli na którymś czas się skończył, gra się kończy.
/// Później, jeśli jakiś wektor jest pusty, to zwracamy automatycznie fałsz - dalszy ruch jest niemożliwy.
/// W przeciwnym razie, dla każdej figury gracza aktualnie wykonującego ruch sprawdzamy, czy się może poruszać, sprawdzając i ustawiając przy okazji możliwość bicia dla danej figury.
/// Jeśli po przejrzeniu całego wektora ustalimy, że żadna figura nie może się ruszać, gra się kończy również - zablokowanie figur to drugi sposób na zwycięstwo.
bool State::decideIfGameShouldContinue() {
    std::shared_ptr<Human> humanBlack = std::dynamic_pointer_cast<Human>(getBlack());  //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
    if (humanBlack && humanBlack->getRemainingTime() == pt::seconds(0)) return false;   // jesli czas to 0 sekund - skonczyl sie
    std::shared_ptr<Human> humanWhite = std::dynamic_pointer_cast<Human>(getWhite()); //rzutowanie wskaźnika typu std::shared_ptr<Player> na std::shared_ptr<Human>, wykorzystując std::dynamic_pointer_cast, w celu dostępu do metod dostępowych czasu
    if (humanWhite && humanWhite->getRemainingTime() == pt::seconds(0)) return false;   // jesli czas to 0 sekund - skonczyl sie
    if (getWhite()->getPiecesLeft().empty() || getWhite()->getPiecesLeft().empty()) return false;           // jesli wektor jakis jest pusty, nie ma po co grac
    bool somePieceCanMove=false;                // zaloz, ze zadna figura nie moze sie poruszac
    if (!getCurrentColor())  {              // w zaleznosci od koloru
        for (const PiecePtr& p: getWhite()->getPiecesLeft()) {      // dla kazdej figury w wektorze
            p->setIsAbleToCapture(false);           // funkcja przy okazji ustawia kazdej figurze pole isAbleToCapture - na poczatku zakladamy, ze figura nie zbija
            for (int i = 0; i < 64; i++) {
                if (p->checkIfThisMoveIsPossible(shared_from_this(), i)) somePieceCanMove=true; // jesli prawda - to jakas figura mogla sie ruszyc na jakies pole
            }
        }
    }
    else {      // analogicznie jak wyzej - tyle ze dla czarnych
        for (const PiecePtr& p: getBlack()->getPiecesLeft()) {  // dla kazdej figury w wektorze
            p->setIsAbleToCapture(false);             // funkcja przy okazji ustawia kazdej figurze pole isAbleToCapture - na poczatku zakladamy, ze figura nie zbija
            for (int i = 0; i < 64; i++) {
                if (p->checkIfThisMoveIsPossible(shared_from_this(), i)) somePieceCanMove=true; // jesli prawda - to jakas figura mogla sie ruszyc na jakies pole
            }
        }
    }
    return somePieceCanMove;        // zwroc, czy czy figura gracza wykonujacego ruch moze sie ruszac - jesli nie, to nie ma jak grac
}
///destruktor
State::~State() = default;












