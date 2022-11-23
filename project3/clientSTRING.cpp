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
// размер буфера
#define BUF_SIZE 64

using namespace std;

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
    int from_len;
    char buf[BUF_SIZE] = {0};
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
    string msg, msg2;
    do
    {
        getline(cin, msg);
        int msg_size = msg.size();
        // отправка запроса серверу
        send(s, (char*) &msg[0], msg_size, 0);

        /*
        получение ответа от сервера
        передаем сокет, буфер, размер буфера
        и int flags = 0 - данные будут
        удалены из сокета после приема
        recv() вернет число считанных байтов
        */
        from_len = recv(s, (char*) &buf, BUF_SIZE, 0);
        // запишем 0 в конец полученных данных
        buf[from_len] = 0;
        msg2 = (string) buf;
        
        // интерпретация ответа от сервера
        // if (msg2 == "0")
        //     cout << "There're no roots in real nums" << endl;
        // // !!! РАЗБЕЙ СТРОКУ
        // else if ( == "1")
        // {
        //     cout << "1 root: " << 
        // }
        // else
        // {

        // }
        

        
    } while (msg2 != "bye");

    cout << "exit" << endl;
    closesocket(s);

    return 0;
}