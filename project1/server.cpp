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
    ifstream fR;
    ofstream fA;

    // указатели на строковые литералы (путей)
    const char* nameR = "REQUEST.bin";
    const char* nameA = "ANSWER.bin";

    cout << "server is working\n";

    // открытие файла в бинарном режиме для чтения
    fR.open(nameR, ios::binary);
    // установка указателя в конец файла (0 - смещение)
    fR.seekg(0, ios::end);
    // предыдущий размер файла (инициализируется текущим)
    size_prev = fR.tellg();
    fR.close();

    while (true)
    {
        // снова открываем и ставим указатель в конец
        fR.open(nameR, ios::binary);
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
        /* &p - получить адрес 'p' в памяти
        (char*) - преобразовать его в указатель на char
        первый параметр read() - куда считываем, т.е.
        считаем в структуру p, к которой обратимся через указатель 
        sizeof() - размер в байтах
        второй параметр read() - сколько байт считать */
        fR.read((char*) &p, sizeof(p));
        /* read сместит указатель. мы обновим size_prev - 
        поставим на конец записанных данных */
        size_prev = fR.tellg();
        fR.close();

        cout << p.name << " " << p.height << " " << p.weight << endl;
        double BMI = p.weight / (0.01 * p.height) / (0.01 * p.height);
        if (18.5 <= BMI && BMI < 25) answer = 1;
        if (18.5 > BMI) answer = 0;
        if (25 <= BMI) answer = 2;

        /* записываем ответ для клиента - рассчитанный BMI
        открываем файл в бинарном режиме для записи в конец */
        fA.open(nameA, ios::binary | ios::app);
        fA.write((char*) &answer, sizeof(answer));
        fA.close();
    }
}