#include <vector>
#include <Windows.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <bitset>
#include <conio.h>
#include <string>

using namespace std;

enum GameEvent {
    FieldEmpty,
    FieldFull,
};

struct Observer {
    virtual void newEvent(GameEvent event) = 0;
};
struct Subject {
    vector<Observer*> observers;
    void subscribe(Observer& tmp) {
        observers.push_back(&tmp);
    }
    void update(GameEvent event) {
        for (auto observer : observers) {
            observer->newEvent(event);
        }
    }
};

enum TypeCell
{
    env,
    alive
};
struct Cell
{
    TypeCell type;
    friend ostream& operator<<(ostream& out, const Cell& cell)
    {
        if (cell.type == env) out << '.';
        else if (cell.type == alive) out << '#';
        return out;
    }
};

struct Field1D
{
    int n = 0;
    vector<Cell> cells;
    Field1D(int n) :n(n), cells(vector<Cell>(n)) {}
    int getNum(int pos, TypeCell type = TypeCell::alive, int radius = 1) const
    {
        int count = 0;
        for (int i = pos - radius; i <= pos + radius; i++)
            if (cells[(i + n) % n].type == type)
                count++;
        return count;
    }
    Cell& operator[](int i) { return cells[i]; }
    Cell operator[](int i) const { return cells[i]; } // const вариант для cout
    friend ostream& operator<<(ostream& out, const Field1D& field)
    {
        for (int i = 0; i < field.n; i++)
            out << field[i];
        return out;
    }
};
struct Field2D
{
    int n = 0;
    int m = 0;
    vector<Field1D> cells;
    Field2D() {}
    Field2D(int n, int m) : n(n), m(m), cells(vector<Field1D>(n, Field1D(m))) {}
    int getNum(int posX, int posY, TypeCell type = alive, int radius = 1) const
    {
        int count = 0;
        for (int i = posX - radius; i <= posX + radius; i++)
            count += cells[(i + n) % n].getNum(posY, type, radius);
        return count;
    }
    Field1D& operator[](int i) { return cells[i]; }
    Field1D operator[](int i) const { return cells[i]; }
    friend ostream& operator<<(ostream& out, const Field2D& field)
    {
        for (int i = 0; i < field.n; i++)
            out << field[i] << "\n";
        return out;
    }
};
struct Field3D
{
    int n = 0;
    int m = 0;
    int k = 0;
    vector<Field2D> cells;
    Field3D() {}
    Field3D(int n, int m, int k) : n(n), m(m), k(k), cells(vector<Field2D>(k, Field2D(n, m))) {}

    int getNum(int posZ, int posX, int posY, TypeCell type = alive, int radius = 1) const
    {
        int count = 0;
        for (int i = posZ - radius; i <= posZ + radius; i++)
        {
            count += cells[(i + k) % k].getNum(posX, posY, type, radius);
        }
        return count;
    }
    Field2D& operator[](int i) { return cells[i]; }
    Field2D operator[](int i) const { return cells[i]; }
    friend ostream& operator<<(ostream& out, const Field3D& field)
    {
        for (int i = 0; i < field.k; i++)
            out << i << ":\n" << field[i] << "\n";
        return out;
    }
};

struct iGame : public Subject
{
    int n = 0;
    int m = 0;
    int k = 0;

    int seed = 0; // случайная величина для генератора
    double probability = 0.0;  // вероятность того, что клетка живая
    int dimension = 1; // размерность

    int radius = 1; // радиус проверки, граница включена
    int loneliness = 2; // с этого числа и меньше клетки умирают от одиночества
    int birth_start = 3; // с этого числа и до birth_end появляется живая клетка
    int birth_end = 3;
    int overpopulation = 5; // с этого числа и дальше клетки погибают от перенаселения

    virtual void setGame(double p, int s = 0) = 0;
    virtual void runGame(int numIt) = 0;
    virtual int getCount(TypeCell type = alive) = 0;
    virtual double getProb(TypeCell type = alive) = 0;
    virtual void print(ostream& out) const = 0;


    void run(int numIt) {
        runGame(1);
        int count = getCount();
        if (count == 0) {
            update(FieldEmpty);
        }
    }

    friend ostream& operator<< (ostream& out, const iGame& game) {
        game.print(out);
        return out;
    }
};
struct Game2D : public iGame
{
    Field2D field;
    Field2D fieldNext;
    Game2D() { dimension = 2; }
    Game2D(int n, int m) {
        this->n = n;
        this->m = m;
        dimension = 2;
        field = fieldNext = Field2D(n, m);
    }
    void setGame(double p, int s = 0)
    {
        probability = p;
        seed = s;
        field = Field2D(n, m);
        vector<int> tmp(n * m);
        iota(tmp.begin(), tmp.end(), 0);
        shuffle(tmp.begin(), tmp.end(), std::mt19937(seed));
        for (int i = 0; i < (int)(p * n * m + 0.5); i++)
        {
            int x = tmp[i] / m;
            int y = tmp[i] % m;

            field[x][y].type = TypeCell::alive;
        }
    }
    void runGame(int numIt) override
    {
        for (int it = 0; it < numIt; it++)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < m; j++)
                {
                    int count = field.getNum(i, j, alive, radius);
                    fieldNext[i][j].type = field[i][j].type;
                    if (count <= loneliness || count >= overpopulation) fieldNext[i][j].type = TypeCell::env;
                    else if (count >= birth_start && count <= birth_end) fieldNext[i][j].type = TypeCell::alive;
                }
            }
            field = fieldNext;
        }
    }
    int getCount(TypeCell type = alive) {
        int count = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (field[i][j].type == type) count++;
            }
        }
        return count;
    }
    double getProb(TypeCell type = alive) {
        int count = getCount();
        double prob;
        prob = (double) count / (n * m);
        return prob;
    }
    void print(ostream& out) const {
        out << field;
    }
};
struct Game3D : public iGame
{
    Field3D field;
    Field3D fieldNext;
    Game3D() { dimension = 3; }
    Game3D(int n, int m, int k) {
        this->n = n;
        this->m = m;
        this->k = k;
        dimension = 3;
        field = fieldNext = Field3D(n, m, k);
    }
    void setGame(double p, int s = 0) {
        probability = p;
        seed = s;
        field = Field3D(n, m, k);
        vector<int> tmp(n * m * k);
        iota(tmp.begin(), tmp.end(), 0);
        shuffle(tmp.begin(), tmp.end(), std::mt19937(seed));

        for (int i = 0; i < (int)(p * n * m * k + 0.5); i++)
        { 
            int z = tmp[i] / (m*n);
            tmp[i] %= m*n;
            int x = tmp[i] / m;
            int y = tmp[i] % m;
            field[z][x][y].type = TypeCell::alive;
        }
    }
    void runGame(int numIt) override  {
        for (int it = 0; it < numIt; it++)
        {
            for (int h = 0; h < k; h++) {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < m; j++)
                    {
                        int count;
                        count = field.getNum(h, i, j, alive, radius);
                        fieldNext[h][i][j].type = field[h][i][j].type;
                        if (count <= loneliness || count >= overpopulation) fieldNext[h][i][j].type = TypeCell::env;
                        else if (count >= birth_start && count <= birth_end) fieldNext[h][i][j].type = TypeCell::alive;
                    }
                }
            }
            field = fieldNext;
        }
    }
    int getCount(TypeCell type = alive) override {
        int count = 0;
        for (int h = 0; h < k; h++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    if (field[h][i][j].type == type) count++;
                }
            }
        }
        return count;
    }
    double getProb(TypeCell type = alive) {
        int count = getCount();
        double prob;
        prob = (double) count / (n * m * k);
        return prob;
    }
    void print(ostream& out) const
    {
        out << field;
    }
};

struct View : public Observer {
    iGame* game;
    int dimension = 0;
    int button = 0;
    int n, m, k;
    double prob;
    string message;

    enum Status {
        gameSetup,
        gameReady,
        gameRun,
        gamePause,
        gameOver
    };
    Status status = gameSetup;

    void setCur(int x, int y = 0) {
        COORD pos = { y, x };
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(output, pos);
    }

    View() {}
    void start() {
        while (1) {
            if (status == gameSetup) {
                    system("cls");
                    cout << "Выберете режим игры:"<<endl;
                    cout << "_2D_"<<endl;
                    cout << "_3D_"<<endl;
                    int pos = 0;
                    setCur(pos + 1);
                    while (1) {
                        if (_kbhit()) {
                            button = _getch();
                            if (button == 'w') {
                                pos = (pos - 1 + 2) % 2;
                                setCur(pos + 1);
                            }
                            else if (button == 's') {
                                pos = (pos + 1) % 2;
                                setCur(pos + 1);
                            }
                            else if (button == 13 || button == 32) {

                                setCur(3);
                                dimension = (int)(pos + 2);

                                if (dimension == 2) {

                                    cout << "Введите размеры поля:"<< endl;
                                    cin >> n >> m;
                                    game = new Game2D(n, m);
                                    cout << "Введите вероятность генерации (от 0 до 1):" << endl;
                                    cin >> prob;
                                    game->setGame(prob);
                                    game->subscribe(*this);
                                }
                                else if (dimension == 3) {

                                    cout << "Введите размеры поля:" << endl;
                                    cin >> n >> m >> k;
                                    game = new Game3D(n, m, k);
                                    cout << "Введите вероятность генерации (от 0 до 1):" << endl;
                                    cin >> prob;
                                    game->setGame(prob);
                                    game->subscribe(*this);
                                }
                                break;
                            }
                        }
                    }

                    system("cls");
                    status = gameReady;
                
            }
            else if (status == gameReady) {
                system("cls");
                cout << *game << endl;
                cout << "Управление:" << endl << endl;
                cout << "ESC    |  выйти из игры" << endl;
                cout << "r      |  начать заново" << endl;
                cout << "ПРОБЕЛ |  начало игры/пауза" << endl << endl;
                cout << "Для старта игры нажмите пробел." << endl;
                _getch();
                system("cls");
                status = gameRun;
            }
            else if (status == gameRun) {
                setCur(0);
                game->run(1);
                cout << *game;
                if (_kbhit()) {
                    button = _getch();
                    if (button == 32) {
                        status = gamePause;
                    }
                    if (button == 'r') {
                        status = gameSetup;
                    }
                    if (button == 27) {
                        system("cls");
                        status = gameOver;
                    }
                }

                Sleep(100);
            }
            else if (status == gamePause) {
                if (_kbhit()) {
                    button = _getch();
                    if (button == 27) {
                        status = gameOver;
                        message = "";
                    }
                    if (button == 32) {
                        status = gameRun;
                    }
                    if (button == 'r') {
                        status = gameSetup;
                    }

                }
            } 
            else if (status == gameOver) {
                system("cls");
                cout << message << endl;
                cout << "Игра окончена."<< endl<<endl;
                cout << "Запустить игру снова?"<<endl;
                cout << "__Да__"<<endl;
                cout << "__Нет_"<<endl;
                int pos = 0;
                setCur(pos + 4);
                while (1) {
                    if (_kbhit()) {
                        button = _getch();
                        if (button == 'w') {
                            pos = (pos - 1 + 2) % 2;
                            setCur(pos + 4);
                        }
                        if (button == 's') {
                            pos = (pos + 1) % 2;
                            setCur(pos + 4);
                        }
                        if (button == 13 || button == 32) {
                            if (pos == 0) {
                                status = gameSetup;
                                break;
                            }
                            else if (pos == 1)
                                break;
                        }
                    }
                }
                if (status != gameSetup) {
                    break;
                }
            }
        }
    }
    void newEvent(GameEvent event) {
        if (event == FieldEmpty) {
            status = gameOver;
            message = "Нет ни одной живой клетки.";
        }
        if (event == FieldFull) {
            status = gameOver;
            message = "Все клетки живые.";
        }
    }
    ~View() {
        delete game;
    }

};

int main()
{
    setlocale(LC_ALL, "Russian");
    View view;
    view.start();

    return 0;
}
