#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <cmath>

#pragma comment(lib, "Ws2_32.lib")

#define SRV_PORT 1234
// размер буфера
#define BUF_SIZE 64

using namespace std;

struct Coefficients
{
    int a, b, c;
} coeffs;

struct Equation {
    int roots;
    double r1, r2;
} eq;

int main() {
    char buff[1024];
    if (WSAStartup(0x0202, (WSADATA*) &buff[0]))
    {
        cout << "Error WSAStartup\n" << WSAGetLastError();
        return -1;
    }

    SOCKET s, s_new;
    // инициализация массива символов
    /*
    примеры:
    x has type int[3] and holds 1,2,3:
    int x[] = {1,2,3};

    y has type int[5] and holds 1,2,3,0,0:
    int y[5] = {1,2,3};

    z has type int[4] and holds 1,0,0,0:
    int z[4] = {1};

    w has type int[3] and holds all zeroes:
    int w[3] = {0};
    */
    char buf[BUF_SIZE] = {0};
    // структуры сокетов для сервера и клиента
    sockaddr_in sin, from_sin;
    s = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(SRV_PORT);
    bind(s, (sockaddr*) &sin, sizeof(sin));

    // создаем очередь запросов длиной 10
    listen(s, 10);

    while (true)
    {
        int from_len = sizeof(from_sin);
        /*
        принимаем соединение из очереди -
        инициализируем сокет s_new
        */
        s_new = accept(s, (sockaddr*) &from_sin, &from_len);
        cout << "new connected client!" << endl;
        while (true)
        {
            // получение запроса без очистки данных (MSG_PEEK)
            from_len = recv(s_new, (char*) &coeffs, sizeof(coeffs), MSG_PEEK);
            // чистим данные и проверяем на наличие строки "bye"
            recv(s_new, (char*) &buf, BUF_SIZE, 0);
            // запишем 0 в конец полученных данных
            buf[from_len] = 0;
            string msg = (string) buf;
            if (msg == "bye")
                break;

            cout << coeffs.a << " " << coeffs.b << " " << coeffs.c << endl;
            // считаем дискриминант
            double D = coeffs.b * coeffs.b - 4 * coeffs.a * coeffs.c;

            // просчет вещ-ых корней (если они есть)
            if (D > 0)
            {
                eq.roots = 2;
                eq.r1 = (-coeffs.b + sqrt(D)) / (2 * coeffs.a);
                eq.r2 = (-coeffs.b - sqrt(D)) / (2 * coeffs.a);
            }
            else if (D == 0)
            {
                eq.roots = 1;
                eq.r1 = -coeffs.b / (2 * coeffs.a);
            }
            else
                eq.roots = 0;

            // отправка ответа
            send(s_new, (char*) &eq, sizeof(eq), 0);
        }
        cout << "client is lost" << endl;
        closesocket(s_new);
    }
    return 0;
}