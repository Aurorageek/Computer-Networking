#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 12345;
const int BufSize = 1024;

SOCKET clientSocket; //����ͻ���socket

DWORD WINAPI recvThread() //������Ϣ�߳�
{
    while (true)
    {
        char buffer[BufSize] = {};//�������ݻ�����
        if (recv(clientSocket, buffer, sizeof(buffer), 0) > 0)
        {
            cout << buffer << endl;
        }
        else if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
        {
            cout << "The Connection is lost!" << endl;
            break;
        }
    }
    
    return 0;
}


int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error initializing Winsock" << std::endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error creating client socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    // ʹ�� inet_pton �� IP ��ַ���ַ���ת��Ϊ��������ʽ
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddr.sin_addr)) <= 0) {
        cerr << "Error converting IP address" << std::endl;
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error connecting to server" << std::endl;
        return 1;
    }
    else
    {
        cout << "connect to server successfully!" << endl;
    }

    //������Ϣ�߳�
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvThread, NULL, 0, 0);
    cout << "Enter your message: " << endl;
    while (true) 
    {
        string message;
        getline(cin, message);
        if (strcmp(message.c_str(), "quit") == 0) //����quit�˳�
        {
            break;
        }

        if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            cerr << "Error sending message" << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
