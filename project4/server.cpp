#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <cmath>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define SRV_PORT 1234

struct Coefficients
{
    int a, b, c;
} coeffs;

struct Equation {
    int roots;
    double r1, r2;
} eq;

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

    SOCKET servSock = socket(AF_INET, SOCK_DGRAM, 0);
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
    servInfo.sin_addr.s_addr = INADDR_ANY;
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

    short packet_size = 0;

    while (true)
    {
        // информация о клиенте
        sockaddr_in clientInfo;
        int clientInfo_size = sizeof(clientInfo);

        // получаем информацию о клиенте (clientInfo) и его сообшение
        packet_size = recvfrom(servSock, (char*) &coeffs, sizeof(coeffs), 0, (sockaddr *) &clientInfo, &clientInfo_size);
        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't send message. Error #" << WSAGetLastError() << endl;
            closesocket(servSock);
            WSACleanup();
            return -1;
        }

        /*
        INET_ADDSTRLEN = длина занимаемого адреса IPv4
        запишем в него строковое
        обозначение (PSTR - вариация строкового литерала)
        адреса клиента (clientInfo.sin_addr)
        */
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientInfo.sin_addr, (PSTR) ipstr, sizeof(ipstr));
        // преобразовываем порт клиента (он в сетевом формате) с помощью ntohs()
        cout << "Received from " << ipstr << ":" << ntohs(clientInfo.sin_port) << endl;
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
        packet_size = sendto(servSock, (char*) &eq, sizeof(eq), 0, (sockaddr*) &clientInfo, sizeof(clientInfo));
        if (packet_size == SOCKET_ERROR)
        {
            cout << "Can't send message. Error #" << WSAGetLastError() << endl;
            closesocket(servSock);
            WSACleanup();
            return -1;
        }
    }
    return 0;
}