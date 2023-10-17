// 服务器端代码

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include<thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


const int PORT = 12345;//端口号
const int MAX_CLIENTS = 3;//最大用户数
int isfree[MAX_CLIENTS] = {};//每一个连接的情况
SOCKET clients[MAX_CLIENTS];//保存客户端的socket的数组

int currentnum = 0; //当前连接的客户数

DWORD WINAPI clientHandler(LPVOID lpParameter) 
{
    int num = (int)lpParameter;
    char sendbuf[1024]; //发送缓冲区
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clients[num], buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "Client " << num << " disconnected" << endl;
            closesocket(clients[num]);
            isfree[num] = 0; // 标记连接为空闲
            currentnum--;
            break;
           
        }
        else {
            cout <<"来自于客户"<<num<<"的消息:" << buffer << endl;
            sprintf_s(sendbuf, sizeof(sendbuf), "%s %d: %s ", "来自",num, buffer); // 格式化发送信息
            // 广播消息给其他所有客户端
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (i != num && isfree[i] == 1) {
                    send(clients[i], sendbuf, sizeof(sendbuf), 0);
                }
            }
        }
    }
    return 0;
}

int find_free()//查询空闲的连接口的索引
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (isfree[i] == 0)//表示该位置连接空闲
        {
            return i;
        }
    }
    exit(EXIT_FAILURE); // 如果没有空闲连接，终止程序
}

int main() {
    //初始化socket库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock" << std::endl;
        return 1;
    }

    //服务器端套接字创建
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//服务器的socket。AF_INET：AF_INET 表示套接字的地址族，通常用于IPv4网络通信。
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating server socket" << endl;
        return 1;
    }
    else
    {
        cout << "Create Server Socket successfully!\n" << endl;

    }

    //绑定soket到服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;//地址类型为Ipv4
    serverAddr.sin_port = htons(PORT);//绑定服务器端的借口号为12345
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Error binding server socket" << endl;
        return 1;
    }
    else
    {
        cout << "bind successfully!" << endl;
    }


    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR) //listen 函数用于将服务器套接字设置为侦听模式，以接受传入的连接请求。
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
            SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);//接受传入的连接请求

            if (clientSocket == INVALID_SOCKET) {
                cerr << "Error accepting client connection" << endl;
                continue;
            }

            int num = find_free();
            isfree[num] = 1;
            clients[num] = clientSocket;

            currentnum++; //当前连接数加1

            

            cout << "Client" <<"  " <<num<<"  " << "connected" << endl;
            if (currentnum == MAX_CLIENTS)cout << "已达到最大数量，无法再创建新的客户端！" << endl;

            //thread clientThread(clientHandler, num, clientSocket);
            HANDLE Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)num, 0, NULL);//创建线程
        }
       
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
