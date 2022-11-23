#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <cmath>

#pragma comment(lib, "Ws2_32.lib")

#define SRV_PORT 1234
#define BUF_SIZE 64

using namespace std;

int a, b, c;

int main() {
    char buff[1024];
    if (WSAStartup(0x0202, (WSADATA*) &buff[0]))
    {
        cout << "Error WSAStartup\n" << WSAGetLastError();
        return -1;
    }

    SOCKET s, s_new;
    char buf[BUF_SIZE] = {0};
    // структуры сокетов для сервера и клиента
    sockaddr_in sin, from_sin;
    s = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(SRV_PORT);
    bind(s, (sockaddr*) &sin, sizeof(sin));

    // сообщение клиенту и от клиента
    string msg, msg2;

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
            // получение запроса
            from_len = recv(s_new, (char*) buf, BUF_SIZE, 0);
            buf[from_len] = 0;
            msg = (string) buf;
            if (msg == "bye")
                break;

            // получаем коэфф-ы и считаем дискриминант
            int shift = 0, res = msg.find(' ', shift);
            string temp;
            // до первого пробела
            temp.assign(msg, shift, res - shift);
            // конвертация строки в число
            a = stoi(temp);

            // увеличиваем сдвиг
            shift = res + 1;
            // второй пробел, ищем от сдвига
            res = msg.find(' ', shift);
            temp.assign(msg, shift, res - shift);
            b = stoi(temp);

            shift = res + 1;
            temp.assign(msg, shift, msg.size() - shift);
            c = stoi(temp);

            cout << a << " " << b << " " << c;

            double D = b * b - 4 * a * c;

            // просчет вещ-ых корней (если они есть)
            if (D > 0)
                msg2 = "2 " + to_string((-b + sqrt(D)) / (2 * a)) + 
                      " " + to_string((-b - sqrt(D)) / (2 * a));
            else if (D == 0)
                msg2 = "1 " + to_string(-b / (2 * a));
            else
                msg2 = "0";
            // отправка ответа msg2
            send(s_new, (char*) &msg2[0], msg2.size(), 0);
        }
        cout << "client is lost" << endl;
        closesocket(s_new);
    }
    return 0;
}