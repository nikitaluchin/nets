#include <iostream>
#include <fstream>
#include <windows.h> // для Sleep()
#include <cmath>

using namespace std;

int a, b, c;
long size_prev;

int main()
{
    // входной и выходной файловые потоки
    ifstream fR;
    ofstream fA;

    // указатели на строковые литералы (путей)
    const char* nameR = "REQUEST.txt";
    const char* nameA = "ANSWER.txt";

    cout << "server is working\n";

    // открытие файла для чтения
    fR.open(nameR);
    // установка указателя в конец файла (0 - смещение)
    fR.seekg(0, ios::end);
    // предыдущий размер файла (инициализируется текущим)
    size_prev = fR.tellg();
    fR.close();

    while (true)
    {
        // снова открываем и ставим указатель в конец
        fR.open(nameR);
        fR.seekg(0, ios::end);

        /* пока в файле не появился запрос
        (размер файла такой же или уменьшился) */
        while (size_prev >= fR.tellg())
        {
            Sleep(100);
            // указатель в конец файла
            fR.seekg(0, ios::end);
        } // вышли из цикла => появился запрос

        /* сместим указатель на место перед новой записью,
        т.е. после 'size_prev' байт от начала файла */
        fR.seekg(size_prev, ios::beg);
        fR >> a >> b >> c;
        /* '>>' делает перенос строки, притом
        не переводит указатель на нее, сделаем это сами
        ios::cur - относительно текущего места
        */
        fR.seekg(1, ios::cur);
        /* обновим size_prev - 
        поставим на конец записанных данных */
        size_prev = fR.tellg();
        fR.close();

        cout << a << " " << b << " " << c << endl;
        float D = b * b - 4 * a * c;

        // открыли файл в режиме добавления в конец
        fA.open(nameA, ios::app);
        
        // просчет корней и запись ответа
        if (D > 0)
            fA << 2 << " " << (-b + sqrt(D)) / (2 * a)
               << " " << (-b - sqrt(D)) / (2 * a) << endl;
        else if (D == 0)
            fA << 1 << " " << -b / (2 * a) << endl;
        else fA << 0 << endl;

        fA.close();
    }
}