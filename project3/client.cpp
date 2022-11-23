/*
1 -2 3 => нет корней
1 2 -8 => 2, -4
1 13 40 => -5, -8
1 -4 4 => 2
1 6 9 => -3
*/

#include <iostream>
// для работы с сокетами
#include <winsock2.h>
// cpp-вектор для хранения строк
#include <string>
// для работы с Windows API
#include <windows.h>

/*
линкер скомпонует библиотеку в исполняемый файл
.exe =
объектные файлы (smth.cpp -> smth.o)
+
библиотеки
*/
#pragma comment(lib, "Ws2_32.lib")

#define SRV_HOST "localhost"
#define SRV_PORT 1234
#define CLNT_PORT 1235

using namespace std;

/*
объявление структуры с коэффициентами
для квадратного трехчлена
*/
struct Coefficients
{
    int a, b, c;
} coeffs;

/*
объявление структуры с ответом
от сервера
*/
struct Equation {
    int roots;
    double r1, r2;
} eq;

int main()
{
    // буфер размером 1 кб
    char buff[1024];
    /*
    инициализация Winsock:
    0x0202 - шестнадцатеричный номер версии интерфейса 2.2
    можно аналогично написать MAKEWORD(2, 2)

    второй аргумент - преобразовываем
    адрес первого элемента буфера в
    указатель на структуру WSADATA, в которую
    будут записаны сведения о конкретной реализации
    интерфейса Winsock
    вместо него можно было бы определить:
    WSADATA wsaData;
    и передать &wsaData 
    */
    if (WSAStartup(0x0202, (WSADATA*) &buff[0]))
    {
        // если иниц-ия не удалась, вернется ненулевое значение
        cout << "Error WSAStartup\n" << WSAGetLastError();
        return -1;
    }
    // объявление сокета
    SOCKET s;
    /*
    объявление указателя на структуру hostent,
    хранящую информацию об узле
    */
    hostent* hp;
    // объявление структур сокетов для клиента и сервера
    sockaddr_in clnt_sin, srv_sin;
    /*
    создание сокета для протокола IPv4 (AF_INET)
    с режимом потокового соединения (SOCK_STREAM)
    0 - выбор протокола по умолчанию для заданного
    потокового соединения
    (для SOCK_STREAM будет выбран TCP)
    */
    s = socket(AF_INET, SOCK_STREAM, 0);
    // используем IPv4
    clnt_sin.sin_family = AF_INET;
    // указание IP-адреса. 0 ~ INADDR_ANY - любой IP
    clnt_sin.sin_addr.s_addr = 0;
    // htons() преобразует порт в сетевой формат
    clnt_sin.sin_port = htons(CLNT_PORT);
    // привязываем заполненную структуру к сокету
    bind(s, (sockaddr*) &clnt_sin, sizeof(clnt_sin));
    // получаем IP по доменному имени сервера
    hp = gethostbyname(SRV_HOST);
    // заполняем структуру sockaddr_in для сервера
    srv_sin.sin_port = htons(SRV_PORT);
    srv_sin.sin_family = AF_INET;
    // заполняем адрес сервера
    ((unsigned long*) &srv_sin.sin_addr)[0] = 
        ((unsigned long**) hp -> h_addr_list)[0][0];
    connect(s, (sockaddr*) &srv_sin, sizeof(srv_sin));

    while (true)
    {
        cout << "Input coeffs: a b c (or enter all zeros to exit)" << endl;
        cin >> coeffs.a >> coeffs.b >> coeffs.c;

        if (!coeffs.a && !coeffs.b && !coeffs.c)
        {
            send(s, (char*) "bye", sizeof("bye"), 0);
            closesocket(s);
            cout << "exiting..." << endl;
            return 0;
        }

        // отправка запроса серверу
        send(s, (char*) &coeffs, sizeof(coeffs), 0);

        /*
        получение ответа от сервера
        передаем сокет, буфер, размер буфера
        и int flags = 0 - данные будут
        удалены из сокета после приема
        recv() вернет число считанных байтов
        */
        int from_len = recv(s, (char*) &eq, sizeof(eq), 0);
        
        // интерпретация ответа от сервера
        if (eq.roots == 0)
            cout << "There're no roots in real nums" << endl;
        else if (eq.roots == 1)
        {
            cout << "1 root: " << eq.r1 << endl;
        }
        else
        {
            cout << "2 roots: " << eq.r1 << " " << eq.r2 << endl;
        }
        cout << endl;
    }

    return 0;
}