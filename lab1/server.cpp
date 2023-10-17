// �������˴���

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include<thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


const int PORT = 12345;//�˿ں�
const int MAX_CLIENTS = 3;//����û���
int isfree[MAX_CLIENTS] = {};//ÿһ�����ӵ����
SOCKET clients[MAX_CLIENTS];//����ͻ��˵�socket������

int currentnum = 0; //��ǰ���ӵĿͻ���

DWORD WINAPI clientHandler(LPVOID lpParameter) 
{
    int num = (int)lpParameter;
    char sendbuf[1024]; //���ͻ�����
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clients[num], buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client " << num << " disconnected" << endl;
            closesocket(clients[num]);
            isfree[num] = 0; // �������Ϊ����
            currentnum--;
            break;
           
        }
        else {
            cout <<"�����ڿͻ�"<<num<<"����Ϣ:" << buffer << endl;
            sprintf_s(sendbuf, sizeof(sendbuf), "%s %d: %s ", "����",num, buffer); // ��ʽ��������Ϣ
            // �㲥��Ϣ���������пͻ���
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (i != num && isfree[i] == 1) {
                    send(clients[i], sendbuf, sizeof(sendbuf), 0);
                }
            }
        }
    }
    return 0;
}

int find_free()//��ѯ���е����ӿڵ�����
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (isfree[i] == 0)//��ʾ��λ�����ӿ���
        {
            return i;
        }
    }
    exit(EXIT_FAILURE); // ���û�п������ӣ���ֹ����
}

int main() {
    //��ʼ��socket��
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock" << std::endl;
        return 1;
    }

    //���������׽��ִ���
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//��������socket��AF_INET��AF_INET ��ʾ�׽��ֵĵ�ַ�壬ͨ������IPv4����ͨ�š�
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating server socket" << endl;
        return 1;
    }
    else
    {
        cout << "Create Server Socket successfully!\n" << endl;

    }

    //��soket����������ַ
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;//��ַ����ΪIpv4
    serverAddr.sin_port = htons(PORT);//�󶨷������˵Ľ�ں�Ϊ12345
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error binding server socket" << endl;
        return 1;
    }
    else
    {
        cout << "bind successfully!" << endl;
    }


    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) //listen �������ڽ��������׽�������Ϊ����ģʽ���Խ��ܴ������������
    {
        cerr << "Error listening on server socket" << endl;
        return 1;
    }

    cout << "Server listening on port " << PORT << endl;



    while (true) 
    {
       
        if ((currentnum < MAX_CLIENTS))
        {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);//���ܴ������������

            if (clientSocket == INVALID_SOCKET) {
                cerr << "Error accepting client connection" << endl;
                continue;
            }

            int num = find_free();
            isfree[num] = 1;
            clients[num] = clientSocket;

            currentnum++; //��ǰ��������1

            

            cout << "Client" <<"  " <<num<<"  " << "connected" << endl;
            if (currentnum == MAX_CLIENTS)cout << "�Ѵﵽ����������޷��ٴ����µĿͻ��ˣ�" << endl;

            //thread clientThread(clientHandler, num, clientSocket);
            HANDLE Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)num, 0, NULL);//�����߳�
        }
       
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
