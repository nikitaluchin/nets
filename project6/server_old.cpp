#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define SRV_PORT 1234
#define BUF_SIZE 64

using namespace std;

struct OrderAndCost
{
    int productid;
    int amount;
};

#define PRINTUSERS if (nclients)\
cout << "User online " << nclients << endl;\
else cout << "No user online\n";

DWORD WINAPI ConToClient(LPVOID client_socket);

int nclients = 0;

int main() {
    
    char buff[1024];

    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr.s_addr = 0;
    servInfo.sin_port = htons(SRV_PORT);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Error socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Binding socket to server info is OK" << endl;

    erStat = listen(ServSock, 0x100);
    if (erStat != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Listening..." << endl;
    }

    OrderAndCost* msg = new OrderAndCost;
    float result;

    SOCKET ClientConn;
    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;

    char buf[BUF_SIZE] = { 0 };
    while ((ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size))) {
        if (ClientConn == INVALID_SOCKET) {
            cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
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
        CreateThread(NULL, NULL, ConToClient, &ClientConn, NULL, &thID);

    }
    return 0;
}

DWORD WINAPI ConToClient(LPVOID ClientConn)
{
    SOCKET my_sock;
    int len;
    my_sock = ((SOCKET*)ClientConn)[0];
    char buf[BUF_SIZE] = { 0 };

    OrderAndCost* msg = new OrderAndCost;
    float result;
    short packet_size = 0;

    while (true) {
        packet_size = recv(my_sock, (char*)&buf, BUF_SIZE, 0);
        buf[packet_size] = 0;
        memcpy(msg, buf, sizeof(msg));
        cout << "Message recieved, сalculating the result from: " << msg->productid << " " << msg->amount << endl;

        if (msg->amount >= 0 && msg->productid >= 0)
        {
            result = msg->amount * msg->productid;
            cout << result << endl;
        }
        else break;

        packet_size = send(my_sock, (char*)&result, sizeof(result), 0);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(my_sock);
            WSACleanup();
            return 1;
        }
    }

    result = -1;
    packet_size = send(my_sock, (char*)&result, sizeof(result), 0);
    if (packet_size == SOCKET_ERROR) {
        cout << "Can't send message. Error # " << WSAGetLastError() << endl;
        closesocket(my_sock);
        WSACleanup();
        return 1;
    }

    cout << "The client has disconnected" << endl;
    closesocket(my_sock);

}