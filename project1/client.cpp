#include <iostream>
#include <fstream>
#include <windows.h> // для Sleep()

using namespace std;

struct Person
{
    char name[25];
    int height;
    int weight;
} p;

int answer;
long size_prev;

int main()
{
    // входной и выходной файловые потоки
    ofstream fR;
    ifstream fA;

    // указатели на строковые литералы путей
    const char* nameR = "REQUEST.bin";
    const char* nameA = "ANSWER.bin";

    while (true)
    {
        cout << "Enter request: name, height & weight" << endl;
        cin >> p.name >> p.height >> p.weight;

        // открываем в бинарном режиме для записи в конец
        fR.open(nameR, ios::binary | ios::app);
        /* записываем данные в файл
        (преобразовываем структуру в указатель на массив символов) */
        fR.write((char*) &p, sizeof(p));
        fR.close();

        // открываем файл, где ожидаем увидеть ответ от сервера
        fA.open(nameA, ios::binary);
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
        // считываем данные в answer
        fA.read((char*) &answer, sizeof(answer));
        fA.close();

        // интерпретация результата
        switch (answer)
        {
            case 0:
                cout << "Less than normal\n\n";
                break;

            case 1:
                cout << "Normal\n\n";
                break;

            case 2:
                cout << "More than normal\n\n";
                break;

            default:
                cout << "ERROR\n\n";
                break;
        }
    }
    return 0;
}