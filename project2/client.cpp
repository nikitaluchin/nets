/*
1 -2 3 => нет корней
1 2 -8 => 2, -4
1 13 40 => -5, -8
1 -4 4 => 2
1 6 9 => -3
*/

#include <iostream>
#include <fstream>
#include <windows.h> // для Sleep()

using namespace std;

int roots, sol1, sol2, a, b, c;
long size_prev;

int main()
{
    // входной и выходной файловые потоки
    ofstream fR;
    ifstream fA;

    // указатели на строковые литералы путей
    const char* nameR = "REQUEST.txt";
    const char* nameA = "ANSWER.txt";

    while (true)
    {
        cout << "Enter coeffs: a b c" << endl;
        cin >> a >> b >> c;

        // открываем в режиме записи в конец
        fR.open(nameR, ios::app);
        /* записываем данные в файл */
        fR << a << " " << b << " " << c << endl;
        fR.close();

        // открываем файл, где ожидаем увидеть ответ от сервера
        fA.open(nameA);
        // указатель в конец
        fA.seekg(0, ios::end);
        // запомнили размер файла
        size_prev = fA.tellg();

        // ждем увеличения размера файла
        while (size_prev >= fA.tellg())
        {
            Sleep(100);
            // указатель в конец файла
            fA.seekg(0, ios::end);
        }

        // ставим указатель на начало данных ответа сервера
        fA.seekg(size_prev, ios::beg);
        // считываем число корней
        fA >> roots;

        // интерпретация ответа от сервера
        if (!roots)
            cout << "There're no roots in real nums" << endl << endl;
        else if (roots == 1)
        {
            fA >> sol1;
            cout << "1 root: " << sol1 << endl << endl;
        }
        else
        {
            fA >> sol1 >> sol2;
            cout << "2 roots: " << sol1 << " " << sol2 << endl << endl;
        }

        fA.close();
    }
    return 0;
}