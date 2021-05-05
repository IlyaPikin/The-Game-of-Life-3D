#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <Windows.h>
#include <conio.h>
#include <time.h>
using namespace std;
COORD position;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

struct iGame
{
    int n=0, m=0, k=0;
    int seed = time(NULL); // случайная величина для генератора
    double probability=0; // вероятность того, что клетка живая

    virtual void runGame(int numIt) = 0;
};

struct Game2d :iGame
{
    char** field;
    char** fieldCopy;
    Game2d() {};

    void setField() {
        field = new char* [n + 2];
        fieldCopy = new char* [n + 2]; //поле копия
        for (int i = 0; i < n + 2; i++) {
            field[i] = new char[m + 2];
            for (int j = 0; j < m + 2; j++) {
                if (i == 0 || i == n + 1 || j == 0 || j == m + 1) {
                    field[i][j] = 'X';
                }
                else {
                    if (rand() % 100 <= probability*100) {
                        field[i][j] = 'O';
                    }
                    else {
                        field[i][j] = '.';
                    }
                }
            }

            fieldCopy[i] = new char[m + 2]; //копия
        }
    }
    void draw()
    {
        for (int i = 0; i < n + 2; i++) {
            for (int j = 0; j < m + 2; j++) {
                position.X = i;
                position.Y = j;
                SetConsoleCursorPosition(hConsole, position);
                if (field[i][j] == 'X') {
                    SetConsoleTextAttribute(hConsole, (WORD)((1 << 4) | 11));
                    putchar(field[i][j]);

                }
                else if (field[i][j] == 'O') {
                    SetConsoleTextAttribute(hConsole, (WORD)((10 << 4) | 0));
                    putchar(field[i][j]);

                }
                else if (field[i][j] == '.') {
                    SetConsoleTextAttribute(hConsole, (WORD)((1 << 4) | 6));
                    putchar(field[i][j]);

                }
            }

        }
    }
    int getCount(int x1, int y1) {  
        int count = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (fieldCopy[x1 + i][y1 + j] == 'O')
                        count++;
                }
            }
        return count;
    }
    void runGame(int numIt) override
    {
        srand(seed);
        setField();
        system("cls");
        draw();
        Sleep(1000);
        // игра, проход numIt итераций 
        for (int count = 0; count < numIt; count++) {
            // копирование поля
            for (int i = 0; i < n+2; i++) {
                for (int j = 0; j < m+2; j++) {
                    fieldCopy[i][j] = field[i][j];
                }
            }
            // анализ копии поля и заполнение основного поля
            for (int i = 1; i < n+1; i++) {
                for (int j = 1; j < m+1; j++) {
                    if (fieldCopy[i][j] == 'O' && (getCount(i, j) == 3 || getCount(i, j) == 4)) {
                        field[i][j] = 'O';
                    }
                    else if (fieldCopy[i][j] == '.' && getCount(i, j) == 3) {
                        field[i][j] = 'O';
                    }
                    else {
                        field[i][j] = '.';
                    }
                }
            }
            draw();
            Sleep(100);
        }
        //draw();
    }

    ~Game2d(){
        for (int i = 0; i < n + 2; i++) {
            delete[] field[i];
            delete[] fieldCopy[i];
        }
        delete[] field;
        delete[] fieldCopy;
    }
};

struct Game3d :iGame
{
    char*** field;
    char*** fieldCopy;
    Game3d() {};

    void setField() {
        field = new char** [n + 2];
        fieldCopy = new char** [n + 2]; //поле копия
        for (int i = 0; i < n + 2; i++) {
            field[i] = new char*[m + 2];
            fieldCopy[i] = new char* [m + 2]; //копия
            for (int j = 0; j < m + 2; j++) {
                field[i][j] = new char [k + 2];
                fieldCopy[i][j] = new char [k + 2]; //копия
                for (int l=0; l < k+2; l++) {
                    if (i == 0 || i == n + 1 || j == 0 || j == m + 1 || l==0 || l==k+1) {
                        field[i][j][l] = 'X';
                    }
                    else {
                        if (rand() % 100 <= probability * 100) {
                            field[i][j][l] = 'O';
                        }
                        else {
                            field[i][j][l] = '.';
                        }
                    }
                }
            }
        }
    }
    void draw()
    {
        for (int z = 1; z < k + 1; z++) {
            for (int i = 0; i < n + 2; i++) {
                for (int j = 0; j < m + 2; j++) {
                    position.X = j; 
                    position.Y = i + (n + 2) * (z - 1); // (n+2)*(z-1) - смещение по оси для отрисовки каждого отдельного слоя

                    SetConsoleCursorPosition(hConsole, position);
                    if (field[i][j][z] == 'X') {
                        SetConsoleTextAttribute(hConsole, (WORD)((1 << 4) | 11));
                        putchar(field[i][j][z]);

                    }
                    else if (field[i][j][z] == 'O') {
                        SetConsoleTextAttribute(hConsole, (WORD)((10 << 4) | 0));
                        putchar(field[i][j][z]);

                    }
                    else if (field[i][j][z] == '.') {
                        SetConsoleTextAttribute(hConsole, (WORD)((1 << 4) | 6));
                        putchar(field[i][j][z]);

                    }
                }

            }
        }
    }
    int getCount(int x1, int y1, int z1) {
        int count = 0;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                for (int l = -1; l <= 1; l++) {
                    if (fieldCopy[x1 + i][y1 + j][z1 + l] == 'O')
                        count++;
                }
            }
        }
        return count;
    }
    void editing() {
        int x = 1, y = 1, z = 1;
        while (1) {
            char c = _getch();
            if (c == 'a') {
                y = 1 + ((y - 1) - 1 + m) % m;
            }
            else if (c == 'd') {
                y = 1 + ((y - 1) + 1 + m) % m;
            }
            else if (c == 'w') {
                x = 1 + ((x - 1) - 1 + n) % n;
            }
            else if (c == 's') {
                x = 1 + ((x - 1) + 1 + n) % n;
            }
            else if (c=='e') { // e - вперёд по оси z
                z = 1+((z-1) + 1 + k) % k;
            }
            else if (c=='q') { // q - назад по оси z
                z = 1 + ((z-1) - 1 + k) % k;
            } 
            else if (c == ' ' && (x >= 1) && (x <= n) && (y >= 1) && (y <= m) && z>=1 && z<=k) {
                if (field[x][y][z] == 'O') {
                    field[x][y][z] = '.';
                }
                else if (field[x][y][z] == '.') {
                    field[x][y][z] = 'O';
                }
            }
            else if (c == 13) // enter
            {
                break;
            }
            draw();
        }
    }
    void runGame(int numIt) override
    {
        srand(seed);
        setField();
        system("cls");
        draw();
        editing();
        Sleep(1000);
        // игра, проход numIt итераций 
       for (int count = 0; count < numIt; count++) {
           // копирование поля
           for (int i = 0; i < n + 2; i++) 
               for (int j = 0; j < m + 2; j++) 
                   for(int l = 0; l < k + 2; l++)
                   fieldCopy[i][j][l] = field[i][j][l];
   
           // анализ копии поля и заполнение основного поля
           for (int i = 1; i < n + 1; i++) {
               for (int j = 1; j < m + 1; j++) {
                   for (int l = 1; l < k + 1; l++) {
                       if (fieldCopy[i][j][l] == 'O' && getCount(i, j, l) >= 6 && getCount(i, j, l) <= 11) {
                           field[i][j][l] = 'O';
                       }
                       else if (fieldCopy[i][j][l] == '.' && getCount(i, j, l) >= 7 && getCount(i, j, l) <= 9) {
                           field[i][j][l] = 'O';
                       }
                       else {
                           field[i][j][l] = '.';
                       }
                   }
               }
           }
           draw();
           Sleep(100);
       }
        //draw();
    }

    ~Game3d() {
        for (int i = 0; i < n + 2; i++) {
            for (int j = 0; j < m + 2; j++) {
                delete[] field[i][j];
                delete[] fieldCopy[i][j];
            }
            delete[] field[i];
            delete[] fieldCopy[i];
        }
        delete[] field;
        delete[] fieldCopy;
    }
};

int main()
{
    SetConsoleTitle(TEXT("The game of life"));
    system("color 1f");
    setlocale(LC_ALL, "Russian");
    cout << "__________Игра Жизнь__________" << endl<<"Выберете режим игры (WASD, ENTER):";
    cout << endl << "1. 2D режим" << endl << "2. 3D режим";
    position.X = 0;
    position.Y = 2;
    SetConsoleCursorPosition(hConsole, position);
    iGame* newGame = 0;
    while (1) {
        char c = _getch();
        if (c == 'w') {
            position.Y = 2+(position.Y-1)%2;
            SetConsoleCursorPosition(hConsole, position);
        }
        else if (c == 's') {
            position.Y = 2 + (position.Y +1) % 2;
            SetConsoleCursorPosition(hConsole, position);
        }
        else if (c == 'a') {
            position.X = (position.X-1+11)%11;
            SetConsoleCursorPosition(hConsole, position);
        }
        else if (c == 'd') {
            position.X = (position.X + 1) % 11;
            SetConsoleCursorPosition(hConsole, position);
        }
        else if (c == 13) {
            if (position.Y==2) {
                newGame = new Game2d;
                system("cls");
                cout << "__________Игра Жизнь__________" << endl << "Введите размеры поля:" << endl;
                cin >> newGame->n >> newGame->m;
            }
            else if (position.Y == 3) {
                newGame = new Game3d;
                system("cls");
                cout << "__________Игра Жизнь__________" << endl << "Введите размеры поля:" << endl;
                cin >> newGame->n >> newGame->m >> newGame->k;
            }
            //SetConsoleCursorPosition(hConsole, position);
            break;
        }
    }
    cout << endl << "Введите число итераций:" << endl;
    int numIt;
    cin >> numIt;
    cout << endl << "Введите параметр вероятности генерации:" << endl;
    cin >> newGame->probability;
    newGame->runGame(numIt);

    return 0;
}
