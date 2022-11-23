#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define SRV_PORT 1234
#define BUF_SIZE 64

using namespace std;

// вопрос клиенту
const string QUEST = "Who are you?\n";

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
        msg = QUEST;
        while (true)
        {
            // отправка сообщения msg
            send(s_new, (char*) &msg[0], msg.size(), 0);
            // получение ответа
            from_len = recv(s_new, (char*) buf, BUF_SIZE, 0);
            buf[from_len] = 0;
            msg2 = (string) buf;
            cout << msg2 << endl;
            if (msg2 == "bye")
                break;
            getline(cin, msg);
        }
        cout << "client is lost" << endl;
        closesocket(s_new);
    }
    return 0;
}