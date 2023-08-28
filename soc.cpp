#include "soc.h"

int CreateSocket()
{
    int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdSocket == -1)
    {
        printf("[PID: %d] socket creation failed: %s\n", getpid(), strerror(errno));
        exit(-1);
    }

    return fdSocket;
}

sockaddr_in& RetrieveServerAddr()
{
    static sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVERPORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
    return serverAddr;
}

sockaddr_in& ConnectToServer(int fdSocket)
{
    sockaddr_in& serverAddr = RetrieveServerAddr();
    if (connect(fdSocket, (sockaddr*)&serverAddr, sizeof(sockaddr_in)) == -1)
    {
        printf("[PID: %d] connect failed: %s\n", getpid(), strerror(errno));
        exit(-1);
    }

    return serverAddr;
}

int SendMessage(int fdSocket, const char* pMsg)
{
    if (send(fdSocket, (void*)pMsg, strlen(pMsg)+1, 0) == -1)
    {
        printf("[PID: %d] send failed: %s\n", getpid(), strerror(errno));
        exit(-1);
    }

    return 1;
}

int ReceiveMessage(int fdSocket, char* pReadBuffer)
{
    if (recv(fdSocket, pReadBuffer, BUFFERLENGTH, 0) == -1)
    {
        printf("[PID: %d] receive failed: %s\n", getpid(), strerror(errno));
        exit(-1);
    }

    return 1;
}