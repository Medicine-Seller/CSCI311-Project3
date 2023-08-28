#ifndef SOC_H
#define SOC_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVERPORT 31277
#define SERVERIP "199.17.28.80"
#define BUFFERLENGTH 1024


// Create a socket
int CreateSocket();

// Send message to socket
int SendMessage(int fdSocket, const char* pMsg);

// Receive message from socket
int ReceiveMessage(int fdSocket, char* pReadBuffer);

// Retrieve ServerC address information
sockaddr_in& RetrieveServerAddr();

// Connect to ServerC
sockaddr_in& ConnectToServer(int fdSocket);

#endif

