#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include <iomanip>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char** argv)
{
    setlocale(0, "");
    system("title CLIENT SIDE");

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << "\n";
        return 11;
    }

    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const char* ip = "localhost";

    addrinfo* result = NULL;
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);

    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 12;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            cout << "socket failed with error: " << WSAGetLastError() << "\n";
            WSACleanup();
            return 13;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "невозможно подключиться к серверу. убедитесь, что процесс сервера запущен!\n";
        WSACleanup();
        return 14;
    }

    //////////////////////////////////////////////////////////

    while (true)
    {
        string message;
        cout << "Please, enter a digit to server(for example: 3,5) : ";
        getline(cin, message);

        iResult = send(ConnectSocket, message.c_str(), (int)message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 15;
        }


        char answer[DEFAULT_BUFLEN];

        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        answer[iResult] = '\0';
        double digit = stod(answer);
        if (iResult > 0) {
            cout << "сервер ответил: " <<setprecision(4)<< digit << "\n";
        }
        else if (iResult == 0)
            cout << "соединение с сервером закрыто.\n";
        else
            cout << "recv failed with error: " << WSAGetLastError() << "\n";
    }

    closesocket(ConnectSocket);
    WSACleanup();
}