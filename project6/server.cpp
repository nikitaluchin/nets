#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <cmath>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define SRV_PORT 1234

#define PRINTUSERS if (nclients)\
cout << "User online " << nclients << endl;\
else cout << "No user online\n";

DWORD WINAPI ConToClient(LPVOID client_socket);

int nclients = 0;

struct Coefficients
{
    int a, b, c;
};

struct Equation {
    int roots;
    double r1, r2;
};

int main() {
    
    WSADATA wsData;
    int resCode = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (resCode != 0)
    {
        cout << "Error WinSock initializaion #" << WSAGetLastError();
        return -1;
    }
    else
        cout << "WinSock initialization OK" << endl;

    SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
    if (servSock == INVALID_SOCKET)
    {
        cout << "Error initialization socket #" << WSAGetLastError() << endl;
        closesocket(servSock);
        WSACleanup();
        return -1;
    }
    else
        cout << "Server socket initialization OK" << endl;

    // информация о сервере
    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    // готовы принимать сообщения от любого адреса (INADDR_ANY)
    servInfo.sin_addr.s_addr = 0;
    servInfo.sin_port = htons(SRV_PORT);

    // привязываем заполненную структуру к сокету
    resCode = bind(servSock, (sockaddr*) &servInfo, sizeof(servInfo));
    if (resCode != 0)
    {
        cout << "Error Socket binding to server. Error #" << WSAGetLastError() << endl;
        closesocket(servSock);
        WSACleanup();
        return -1;
    }
    else
        cout << "Binding socket to Server info OK" << endl << endl;

    // 0x100 (16-ая сист. счисл.) = 256 (10)
    resCode = listen(servSock, 0x100);
    if (resCode != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(servSock);
        WSACleanup();
        return -1;
    }
    else
    {
        cout << "Listening..." << endl;
    }

    SOCKET clientConn;
    // информация о клиенте
    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;

    while ((clientConn = accept(servSock, (sockaddr*)&clientInfo, &clientInfo_size)))
    {
        if (clientConn == INVALID_SOCKET) {
            cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
            closesocket(servSock);
            closesocket(clientConn);
            WSACleanup();
            return 1;
        }
        else
            cout << "Connection to a client established successfully" << endl;

        nclients++;
        cout << "new connection!\n";
        PRINTUSERS
        cout << endl;

        DWORD thID;
        CreateThread(NULL, NULL, ConToClient, &clientConn, NULL, &thID);
    }
    return 0;
}

DWORD WINAPI ConToClient(LPVOID clientConn)
{
    SOCKET sock;
    sock = ((SOCKET*) clientConn)[0];
    short packet_size = 0;
    Coefficients coeffs;
    Equation eq;

    while (true)
    {
        // получаем информацию о клиенте (clientInfo) и его сообшение
        packet_size = recv(sock, (char*) &coeffs, sizeof(coeffs), 0);

        // прерывание работы в случае нулевых коэффициентов
        if (!coeffs.a && !coeffs.b && !coeffs.c)
            break;

        cout << "Coeffs: " << coeffs.a << ", " << coeffs.b << ", " << coeffs.c << endl; 

        // считаем дискриминант
        double D = coeffs.b * coeffs.b - 4 * coeffs.a * coeffs.c;

        // просчет вещ-ых корней (если они есть)
        if (D > 0)
        {
            eq.roots = 2;
            eq.r1 = (-coeffs.b + sqrt(D)) / (2 * coeffs.a);
            eq.r2 = (-coeffs.b - sqrt(D)) / (2 * coeffs.a);
            cout << "Roots: " << eq.r1 << ", " << eq.r2 << endl << endl;
        }
        else if (D == 0)
        {
            eq.roots = 1;
            eq.r1 = -coeffs.b / (2 * coeffs.a);
            cout << "Root: " << eq.r1 << endl << endl;
        }
        else
        {
            eq.roots = 0;
            cout << "No roots" << endl << endl;
        }

        // отправляем тому же клиенту, от которого получили
        packet_size = send(sock, (char*) &eq, sizeof(eq), 0);
        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't send message. Error #" << WSAGetLastError() << endl;
            closesocket(sock);
            WSACleanup();
            return -1;
        }
    }

    // флаг, указывающий на прерывание работы
    eq.roots = -1;
    packet_size = send(sock, (char*) &eq, sizeof(eq), 0);
    if (packet_size == SOCKET_ERROR) {
        cout << "Can't send message. Error # " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "The client has disconnected" << endl;
    closesocket(sock);
    nclients--;
    PRINTUSERS
}