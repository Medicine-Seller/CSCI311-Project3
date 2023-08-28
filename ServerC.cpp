#include <sys/wait.h>
#include <pthread.h>
#include "soc.h"

// Create a server(ServerG) for client
void CreateServerForClient(int fdClientSocket);

// Wait for child to exit and display status
void WaitForChildExit();

extern "C" void* ServerC(void* ptr)
{
    int sockfd = CreateSocket();
    sockaddr_in& serverAddr = RetrieveServerAddr();

    // Bind to server address
    if (bind(sockfd, (sockaddr*)&serverAddr, sizeof(sockaddr_in)) == -1)
    {
        perror("[ServerC] bind failed");
        exit(-1);
    }

    // Listen to request
    printf("[ServerC] listening...\n");
    if (listen(sockfd, 5) == -1)
    {
        perror("[ServerC] listen failed");
        exit(-1);
    }

    while(true)
    {
        sockaddr_in clientAddr;
        socklen_t sockfdSize = sizeof(sockaddr_in);

        // Accept request
        int fdSocket = accept(sockfd, (sockaddr*)&clientAddr, &sockfdSize);
        if (fdSocket == -1)
        {
            perror("[ServerC] accept failed");
            exit(-1);
        }
        
        // Receive Message from Client/ServerG
        char readBuffer[BUFFERLENGTH+1];
        ReceiveMessage(fdSocket, readBuffer);
        printf("[ServerC] readBuffer: %s\n", readBuffer);

        // Compare readBuffer string
        if (strcmp(readBuffer, "client_connect") == 0)
            CreateServerForClient(fdSocket);
        else if (strcmp(readBuffer, "serverg_exit") == 0)
            WaitForChildExit();
        else if (strcmp(readBuffer, "exit") == 0)
            break;
    }

    printf("[ServerC] thread closed\n");
    close(sockfd);
    return 0;
}

int main(int argc, char* argv[])
{
    pthread_t tServer;

    printf("\n[]=======================================[]\n");
    printf("||  Note: Type \"close\" to close ServerC  ||\n");
    printf("[]=======================================[]\n\n");

    // Create thread for ServerC
    if (pthread_create(&tServer, NULL, ServerC, NULL) != 0)
    {
        perror("[ServerC] failed to create thread");
        return -1;
    }

    printf("[ServerC] thread started\n");
    
    // Wait for "close" input
    char input[64];
    while (strcmp(input, "close") != 0)
        scanf("%63s", input);

    // Connect to server and send "exit" message
    int fdServerSocket = CreateSocket();
    sockaddr_in& serverAddr = ConnectToServer(fdServerSocket);
    SendMessage(fdServerSocket, "exit");

    // Join thread
    if (pthread_join(tServer, NULL) != 0)
        printf("[ServerC] thread failed to join\n");

    close(fdServerSocket);
    printf("[ServerC] exited\n");
    return 0;
}

void CreateServerForClient(int fdClientSocket)
{
    printf("[ServerC] creating child\n");
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("[ServerC] fork failed");
        exit(-1);
    }
    else if (pid == 0)
    {
        char param0[16];
        sprintf(param0, "%d", fdClientSocket);
        if (execl("./ServerG", "ServerG", param0, (char*)NULL))
            perror("[ServerC] execl failed");

        exit(-1);
    }
    printf("[ServerC] child created with pid: %d\n", pid);
}

void WaitForChildExit()
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, 0);

    if (WIFEXITED(status))
        printf("[ServerC] child PID(%d) exit status: %d\n", pid, WEXITSTATUS(status));
    else
        printf("[ServerC] child PID(%d) did not exit successfully\n", pid);
}