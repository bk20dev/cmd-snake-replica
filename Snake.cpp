#include <iostream>
#include <conio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <ctime>
#include <random>
#include <string>
#include <Windows.h>

#include "Color.h"

#include "Meal.h"
#include "Direction.h"
#include "Boost.h"
#include "SnakeBody.h"
#include "Character.h"

// Pamiętaj o tym, że czym mniejsza wartość opóźnienia generowania ramki, tym obraz będzie bardziej migał
#define UPDATE_DELAY 200
#define GAME_HEIGHT 16
#define GAME_WIDTH 32

// Znaki (kod ascii) używane do generowania obrazu
#define GRASS 32
#define SNAKE_HEAD 178
#define SNAKE_PART 178

#define APPLE 254
#define SPIKE 158

//
// Wątek odpowiedzialny za działanie gry

/// <summary>
/// Renderuje (wypisuje w konsoli) planszę gry
/// </summary>
/// <param name="canvas">plansza gry</param>
void render(std::vector<UCharacter*>* canvas)
{
    system("cls"); // wyczyszczenie ekranu

    for (unsigned int t = 0; t < canvas->size(); t++)
    {
        Color::SetMode(GetStdHandle(STD_OUTPUT_HANDLE), (*(*canvas)[t]).color, (*(*canvas)[t]).background); // ustawienie kolorków
        std::cout << (*(*canvas)[t]).h; // wypisanie znaku
        if ((t + 1) % GAME_WIDTH == 0) std::cout << std::endl; // jeżeli jest to ostatni znak, to wstawiamy enter
    }
}

/// <summary>
/// Przesuwa węża w wybranym kierunku
/// </summary>
/// <param name="snake">wektor z wężem</param>
/// <param name="direction">kierunek, w jakim ma zostać przesunięty wąż</param>
void moveSnake(std::vector<USnakeBody>* snake, EDirection* direction)
{
    const size_t size = snake->size();
    if (size <= 0) return; // jeżeli wąż jest pusty, to nic nie rób

    // przesunięcie ciała
    for (int i = size - 1; i > 0; i--)
    {
        if (i - 1 >= 0)
        {
            (*snake)[i].x = (*snake)[i - 1].x;
            (*snake)[i].y = (*snake)[i - 1].y;
        }
    }

    // przesunięcia głowy we właściwym kierunku
    switch (*direction)
    {
    case EDirection::UP:
        (*snake)[0].y--;
        break;
    case EDirection::DOWN:
        (*snake)[0].y++;
        break;
    case EDirection::LEFT:
        (*snake)[0].x--;
        break;
    case EDirection::RIGHT:
        (*snake)[0].x++;
        break;
    }

    // przeniesienie węża na drugą stronę planszy, jeżeli wchodzi w jakieś krawędzie
    if ((*snake)[0].x >= GAME_WIDTH) (*snake)[0].x = 0;
    if ((*snake)[0].x < 0) (*snake)[0].x = GAME_WIDTH - 1;
    if ((*snake)[0].y >= GAME_HEIGHT) (*snake)[0].y = 0;
    if ((*snake)[0].y < 0) (*snake)[0].y = GAME_HEIGHT - 1;
}

/// <summary>
/// Wydłuża węża
/// </summary>
/// <param name="snake">Wektor z wężem</param>
/// <param name="x">pozycja miejsca, w którym ustawiona zostanie nowa część węża</param>
/// <param name="y">pozycja miejsca, w którym ustawiona zostanie nowa część węża</param>
void extendSnake(std::vector<USnakeBody>* snake, const int x, const int y)
{
    USnakeBody part(x, y);

    snake->push_back(part);
}

/// <summary>
/// Umieszcza na planszy nowy boost (jabłko, pułapkę)
/// </summary>
/// <param name="powerups">wektor zawierający boosty</param>
/// <param name="type">typ boosta, jaki ma zostać dodany na planszy</param>
/// <param name="x">współżędna, na której ma zostać umieszczony boost</param>
/// <param name="y">współżędna, na której ma zostać umieszczony boost</param>
void putBooster(std::vector<UBoost> *powerups, EMeal type, const int x, const int y)
{
    UBoost powerup(type, x, y);

    powerups->push_back(powerup);
}

/// <summary>
/// Zwraca obiekt, który znajduje się na wybranych współżędnych
/// </summary>
/// <param name="snake">wektor z wężem</param>
/// <param name="boosters">wektor z boosterami</param>
/// <returns>obiekt, który znajduje się na wybranych współżędnych</returns>
EMeal getBooster(std::vector<USnakeBody>* snake, std::vector<UBoost>* boosters)
{
    // zapisanie kordynatów węża
    int x = (*snake)[0].x;
    int y = (*snake)[0].y;

    // jeżeli kordynaty wskazują na węża, to zwróć "wąż"
    for (unsigned int i = 1; i < snake->size(); i++)
    {
        if ((*snake)[i].x == x && (*snake)[i].y == y) return EMeal::SNAKE;
    }

    // przeszukanie boosterów
    // jeżeli wąż pożera właśnie booster, to go usuwamy i zwracamy typ tego boostera
    for (unsigned int i = 0; i < boosters->size(); i++)
    {
        if ((*boosters)[i].x == x && (*boosters)[i].y == y)
        {
            EMeal type = (*boosters)[i].type;
            boosters->erase(boosters->begin() + i);
            return type;
        }
    }

    return EMeal::AIR;
}

/// <summary>
/// Resetuje grę
/// </summary>
/// <param name="snake">wektor zawierający węża</param>
/// <param name="boosters">wektor zawierający boostery</param>
/// <param name="canvas">plansza gry</param>
void reset(std::vector<USnakeBody>* snake, std::vector<UBoost>* boosters, std::vector<UCharacter*>* canvas)
{
    // usunięcie węża
    snake->erase(snake->begin(), snake->end());

    // rysowanie nowego węża
    for (char i = 0; i < 3; i++)
    {
        extendSnake(snake, GAME_WIDTH / 2, GAME_HEIGHT / 2 + i);
    }

    // umieszczenie jabłek
    for (char i = 0; i < GAME_WIDTH * GAME_HEIGHT / 30; i++)
    {
        putBooster(boosters, EMeal::FRUIT, rand() % GAME_WIDTH, rand() % GAME_HEIGHT);
    }

    // umieszczenie pułapek
    for (char i = 0; i < GAME_WIDTH * GAME_HEIGHT / 60; i++)
    {
        putBooster(boosters, EMeal::TRAP, rand() % GAME_WIDTH, rand() % GAME_HEIGHT);
    }
}

/// <summary>
/// Główna funkcja wątku obsługującego działanie gry
/// </summary>
/// <param name="snake">wektor z wężem</param>
/// <param name="powerups">wektor z boosterami</param>
/// <param name="direction">kierunek, w jakim porusza się wąż</param>
/// <param name="direction">ostatni kierunek, w jakim poruszał się wąż</param>
/// <param name="canvas">plansza gry</param>
/// <param name="do_update">zmienna przechowująca stan logiczny, informujący o konieczności dalszego aktualizowania gry</param>
void update(std::vector<USnakeBody>* snake, std::vector<UBoost> *powerups, EDirection* direction, EDirection* direction_last, std::vector<UCharacter*>* canvas, bool* do_update)
{
    while (*do_update)
    {
        *direction_last = *direction;

        // wypełnienie trawą planszy
        std::fill(canvas->begin(), canvas->end(), new UCharacter(GRASS, EColor::DARK_GREEN));

        // zapisanie ostatniej pozycji końca węża - to się przyda do jego wydłużenia
        const int lastSnakeX = (*snake)[snake->size() - 1].x;
        const int lastSnakeY = (*snake)[snake->size() - 1].y;

        // przesunięcie węża
        moveSnake(snake, direction);

        // narysowanie węża
        // jeżeli jest to jego początek, to głowy zamiast ciała
        for (unsigned int i = 0; i < snake->size(); i++) { (*canvas)[(*snake)[i].y * GAME_WIDTH + (*snake)[i].x] = i == 0 ? new UCharacter(SNAKE_HEAD, EColor::YELLOW) : new UCharacter(SNAKE_PART, EColor::GOLD); }

        // sprawdzenie tego, na czym jest aktualnie wąż
        const EMeal meal = getBooster(snake, powerups);
        if (meal == EMeal::SNAKE || meal == EMeal::TRAP)
        {
            // jeżeli zjada samego siebie lub wpadł w pułapkę, to zakończ grę
            *do_update = false;
            return;
        }
        else if (meal == EMeal::FRUIT)
        {
            // jeżeli zjada owoc, to wydłuż węża
            putBooster(powerups, EMeal::FRUIT, rand() % GAME_WIDTH, rand() % GAME_HEIGHT);
            extendSnake(snake, lastSnakeX, lastSnakeY);
        }

        // wypełnienie planszy boosterami
        for (UBoost powerup : *powerups)
        {
            switch (powerup.type)
            {
            case EMeal::FRUIT:
                (*canvas)[powerup.y * GAME_WIDTH + powerup.x] = new UCharacter(APPLE, EColor::DARK_RED);
                break;
            case EMeal::TRAP:
                (*canvas)[powerup.y * GAME_WIDTH + powerup.x] = new UCharacter(SPIKE, EColor::DARK_PURPLE);
                break;
            }
        }

        render(canvas);

        std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_DELAY));
    }
}

//
// Wątek obsługujący wciśnięcia klawiatury

/// <summary>
/// Obsługuje wciśnięcia klawiatury
/// </summary>
/// <param name="direction">kierunek, w jakim porusza się wąż</param>
void keyPress(EDirection* direction, EDirection* direction_last)
{
    unsigned char key;
    do
    {
        key = _getch(); // pobranie kodu znaku na wejściu
        switch (tolower(key))
        {
        // za każdym razem - jeżeli poprzedni kierunek nie jest przeciwny temu, na jaki chcemy ustawić nowy, to to robimy
        // w skrócie - jeżeli wąż przesuwa się do przodu, nie możemy zawrócić w miejscu i wejść w siebie
        case 'w':
            if(*direction_last != EDirection::DOWN) *direction = EDirection::UP;
            break;
        case 's':
            if (*direction_last != EDirection::UP) *direction = EDirection::DOWN;
            break;
        case 'a':
            if (*direction_last != EDirection::RIGHT) *direction = EDirection::LEFT;
            break;
        case 'd':
            if (*direction_last != EDirection::LEFT) *direction = EDirection::RIGHT;
            break;
        }
    } while (key != 8); // backspace wyłącza grę
}

//
// Main

int main()
{
    srand(time(NULL));

    bool do_update = true;

    std::vector<USnakeBody> snake;
    EDirection direction = EDirection::UP;
    EDirection direction_last = direction;

    std::vector<UBoost> powerups;
    std::vector<UCharacter*> canvas(GAME_HEIGHT * GAME_WIDTH);
    
    reset(&snake, &powerups, &canvas);

    std::thread renderer(update, &snake, &powerups, &direction, &direction_last, &canvas, &do_update);
    std::thread keyboard(keyPress, &direction, &direction_last);

    renderer.join();
    keyboard.join();

    return 0;
}
