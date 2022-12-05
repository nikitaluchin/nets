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
// для работы с Windows API
#include <windows.h>
// для доп. функций (напр-р, inet_pton)
#include <WS2tcpip.h>

/*
линкер скомпонует библиотеку в исполняемый файл
.exe =
объектные файлы (smth.cpp -> smth.o)
+
библиотеки
*/
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define SRV_PORT 1234
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
    /*
    инициализация Winsock:
    0x0202 - шестнадцатеричный номер версии интерфейса 2.2
    можно аналогично написать MAKEWORD(2, 2),
    что мы и сделали

    второй аргумент - структура WSADATA wsData
    в нее будут записаны сведения
    о конкретной реализации интерфейса Winsock
    */
    WSADATA wsData;
    // код результата. не ноль в случае ошибки
    int resCode = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (resCode != 0)
    {
        // выведем ошибку
        cout << "Error WinSock initializaion #" << WSAGetLastError();
        return -1;
    }
    else
        cout << "WinSock initialization OK" << endl;

    /*
    создание сокета для протокола IPv4 (AF_INET)
    с режимом дейтаграммного соединения (SOCK_DGRAM)
    IPPROTO_UDP - выбор протокола UDP для нашего соединения
    */
    SOCKET clntSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clntSock == INVALID_SOCKET)
    {
        cout << "Error initialization socket #" << WSAGetLastError() << endl;
        closesocket(clntSock);
        // освободить выделенную память для Winsock
        WSACleanup();
        return -1;
    }
    else
        cout << "Client socket initialization OK" << endl << endl;

    // структура с информацией о сервере
    sockaddr_in srvInfo;
    // опустошение памяти (на всякий случай)
    ZeroMemory(&srvInfo, sizeof(srvInfo));

    /*
    преобразует локальный IP "127.0.0.1" версии IPv4 (AF_INET)
    в особый численный формат
    и записывает в поле srvInfo sin_addr
    */
    resCode = inet_pton(AF_INET, "10.16.4.26", &srvInfo.sin_addr);
    if (resCode <= 0)
    {
        cout << "Error in IP translation" << endl;
        return -1;
    }
    // указываем версию IP IPv4
    srvInfo.sin_family = AF_INET;
    // htons() преобразует порт в сетевой формат
    srvInfo.sin_port = htons(SRV_PORT);

    // размер отправляемого и получаемого пакета
    short packet_size = 0;

    while (true)
    {
        cout << "Input coeffs: a b c" << "(or enter all zeros to exit)" << endl;
        cin >> coeffs.a >> coeffs.b >> coeffs.c;

        // прерывание работы в случае нулевых коэффициентов
        if (!coeffs.a && !coeffs.b && !coeffs.c)
        {
            cout << "exit" << endl;
            closesocket(clntSock);
            // освободить выделенную память для Winsock
            WSACleanup();
            return 0;
        }

        /*
        передаем сокет,
        преобразовываем указатель на структуру
        в указатель на char (первая ячейка памяти, занимаемая структурой)
        передаем размер памяти структуры
        0 - стандартный флаг
        передаем указатель на структуру
        srvInfo (инф-я о том, куда мы отправляем)
        и ее размер
        */
        packet_size = sendto(clntSock, (char*) &coeffs, sizeof(coeffs), 0, (sockaddr*) &srvInfo, sizeof(srvInfo));

        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't send message to server. Error #" << WSAGetLastError() << endl;
            closesocket(clntSock);
            // освободить выделенную память для Winsock
            WSACleanup();
            return -1;
        }

        /*
        информация о том, от кого получаем ответ
        в данной задаче совпадает с тем, кому
        отправляем
        */
        sockaddr newInfo;
        int newInfoSize = sizeof(newInfo);

        // аналогично sendto, но получаем
        packet_size = recvfrom(clntSock, (char*) &eq, sizeof(eq), 0, (sockaddr*) &newInfo, &newInfoSize);
        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't recieve message from Server. Error #" << WSAGetLastError() << endl;
            closesocket(clntSock);
            // освободить выделенную память для Winsock
            WSACleanup();
            return -1;
        }

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