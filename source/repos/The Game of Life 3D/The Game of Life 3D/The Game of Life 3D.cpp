#include <vector>
#include <Windows.h>
#include <list>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <bitset>
#include <ctime>
using namespace std;
// 0 -------\ Y (m)
//   -------/
// ||
// ||
// ||
// ||
// \/ X (n)
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

struct iGame
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
    virtual void runGame(int numIt) = 0;
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
    void startGame()
    {

    }
};
struct Game3D : public iGame
{
    int count=0;
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
    double getProportion(TypeCell type = alive) {
        double count = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                for (int h = 0; h < k; h++) {
                    if (field[h][i][j].type == type) count++;
                }
            }
        }
        return count / (double)(n*m*k);
    }
    void print(ostream& out) const
    {
        out << field;
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");
    
    Game3D game = Game3D(4, 4, 4);
    
    cout << "Поле 1:" << endl;
    game.field[1][0][1].type = TypeCell::alive;
    game.field[1][2][3].type = TypeCell::alive;
    game.field[2][0][1].type = TypeCell::alive;
    game.field[2][2][2].type = TypeCell::alive;
    game.field[3][2][2].type = TypeCell::alive;
    game.field[3][3][1].type = TypeCell::alive;
    game.print(cout);
    //cout <<endl<< game.getProportion();
    
    cout << "Ответ:" << endl;
    unsigned int start_time = clock();
    for (int i1 = 1; i1 <= 2; i1++) {
        game.radius = i1;
        for (int i2 = 0; i2 <= 25; i2++) {
            game.loneliness = i2;
            for (int i3 = i2 + 1; i3 <= 26; i3++) {
                game.birth_start = i3;
                for (int i4 = i3; i4 <= 26; i4++) {
                    game.birth_end = i4;
                    for (int i5 = i4 + 1; i5 <= 27; i5++) {
                        game.overpopulation = i5;

                        game.runGame(20);
                        double prop = game.getProportion(alive);
                        //cout << prop<<endl;
                        if (prop >= 0.15 && prop <= 0.2) {
                            cout << endl << i1 << " " << i2 << " " << i3 << " " << i4 << " " << i5;
                        }

                        game.setGame(0);
                        game.field[1][0][1].type = TypeCell::alive;
                        game.field[1][2][3].type = TypeCell::alive;
                        game.field[2][0][1].type = TypeCell::alive;
                        game.field[2][2][2].type = TypeCell::alive;
                        game.field[3][2][2].type = TypeCell::alive;
                        game.field[3][3][1].type = TypeCell::alive;
                    }
                }
            }
        }
    }
    unsigned int end_time = clock();
    unsigned int search_time = end_time - start_time;
    cout<< endl<< endl << "Время выполнения: "<< search_time << " секунд" << endl;

    cout << endl <<  "Поле 2:" << endl;
    game.setGame(1);
    game.field[2][0][2].type = TypeCell::env;
    game.field[2][1][0].type = TypeCell::env;
    game.field[3][2][1].type = TypeCell::env;
    game.print(cout);
    //cout << endl << game.getProportion();

    cout << "Ответ:" << endl;
    for (int i1 = 1; i1 <= 2; i1++) {
        game.radius = i1;
        for (int i2 = 0; i2 <= 25; i2++) {
            game.loneliness = i2;
            for (int i3 = i2 + 1; i3 <= 26; i3++) {
                game.birth_start = i3;
                for (int i4 = i3; i4 <= 26; i4++) {
                    game.birth_end = i4;
                    for (int i5 = i4 + 1; i5 <= 27; i5++) {
                        game.overpopulation = i5;

                        game.runGame(20);
                        double prop = game.getProportion(alive);
                        //cout << prop<<endl;
                        if (prop >= 0.15 && prop <= 0.2) {
                            cout << endl << i1 << " " << i2 << " " << i3 << " " << i4 << " " << i5;
                        }

                        game.setGame(1);
                        game.field[2][0][2].type = TypeCell::env;
                        game.field[2][1][0].type = TypeCell::env;
                        game.field[3][2][1].type = TypeCell::env;
                    }
                }
            }
        }
    }


    return 0;
}

// 0) Cell, Field, type Enum, operator <<
// 1) view в данном варианте не нужен, достаточно иметь Field
// 2) operator[] and "operator[][]" and operator(int i, int j, int k)
// 3) iota, random
// 4) view должен быть связан с game, чтобы получать от него "обновления" и взаимодействовать с ним
// 4) например, мы хотим реализовать меню для игры: паузу, донастройку, чтобы игра сообщала о некоторых событиях.

struct iView
{
    char livingCell; // символ "живой" клетки
    char dyingCell; // символ "неживой" клетки
    // ... свобода творчества для реализации и взаимодействия view, можно реализовать 2d draw и использвать его в 3d
};
struct View2d : iView
{
    void draw(char** field, int n, int m)
    {
        system("cls");
        for (int i = 0; i < n; i++)
            fwrite(field[i], sizeof(char), m + 1, stdout);
    }
};
struct View3d : iView
{
    char*** field;
    void setField(char*** field) {};
    void draw()
    {

    }
};