#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // used for close()
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include "msc_model.h"
#include "client.h"
#include "tcp_ip_helpers.h"

int mscSocket = 0;
clientSockets_t bsSockets;

void exitFunction()
{
    // do stuff to make exit graceful

    // closing server socket if open
    if (mscSocket != 0)
    {
        printf("Closing server socket.\n");
        shutdown(mscSocket, SHUT_RDWR);
        close(mscSocket);
    }

    // close client sockets
    client_closeAllSockets(&bsSockets);

    printf("Mobile Switching Center Closed.\n");
}

void signalHandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
        printf("Received SIGINT signal.\n");
        exit(EXIT_SUCCESS);
        break;
    case SIGILL:
        printf("Received SIGILL signal.\n");
        break;
    case SIGABRT:
        printf("Received SIGABRT signal.\n");
        exit(EXIT_SUCCESS);
        return;
    case SIGFPE:
        printf("Received SIGFPE signal.\n");
        break;
    case SIGSEGV:
        printf("Received SIGSEGV signal.\n");
        break;
    case SIGTERM:
        printf("Received SIGTERM signal.\n");
        break;

    default:
        printf("Received unknown signal.\n");
        break;
    }

    exit(EXIT_FAILURE);
}

void *handleBs(void *arg)
{
    socketData_t *bsSocket = (socketData_t *)arg;
    char buffer[TCP_BUFFER_SIZE];
    int bytesReceived = 0;

    // receive bytes from bs socket
    while ((bytesReceived = recv(bsSocket->socket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0'; // null terminate message

        // TODO: process message
    }

    if (bytesReceived == 0)
    {
        printf("BS[%d]: Disconnected.\n", bsSocket->index);
        client_closeSocket(&bsSockets, bsSocket);
    }
    else if (bytesReceived < 0)
    {
        printf("BS[%d]: Receive error\n", bsSocket->index);
        client_closeSocket(&bsSockets, bsSocket);
    }
}

int main(int argc, char *argv[])
{
    char mscId[MAX_ID_SIZE] = "\0";

    // check args for at least 1 arg
    if (argc < 2)
    {
        printf("Usage: ./msc <ID>\n");
        return EXIT_FAILURE;
    }

    // check ID len
    if (strlen(argv[1]) >= MAX_ID_SIZE - 1)
    {
        printf("Max length for ID is %d\n", MAX_ID_SIZE - 1);
        return EXIT_FAILURE;
    }

    // init client sockets
    client_socketsInit(&bsSockets);

    // connect function to handle exits
    atexit(exitFunction);

    // close sockets on segmentation fault
    signal(SIGSEGV, signalHandler);

    // close sockets on abort signal
    signal(SIGABRT, signalHandler);

    // close sockets on interactive attention
    signal(SIGINT, signalHandler);

    // store ID
    strncpy(mscId, argv[1], sizeof(mscId));

    // print hello message
    printf("Starting Mobile Switching Center with ID: %s\n", mscId);

    // create socket
    mscSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mscSocket < 0)
    {
        printf("Failed to create socket.\n");
        return EXIT_FAILURE;
    }
    printf("Socket created.\n");

    // create internet socket address
    unsigned int port = 0;
    struct sockaddr_in serverAddr =
        {
            .sin_addr = AF_INET,
            .sin_addr.s_addr = INADDR_ANY,
            .sin_port = htons(port)};

    // bind socket and addr
    int bindRes = 0;
    for (unsigned int portNum = DYNAMIC_PORT_MIN; portNum <= DYNAMIC_PORT_MAX; portNum++)
    {
        serverAddr.sin_port = htons(portNum);
        bindRes = bind(mscSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        if (bindRes >= 0) // success
        {
            port = portNum;
            break;
        }
    }
    if (bindRes < 0)
    {
        printf("Failed to bind socket and address.\n");
        return EXIT_FAILURE;
    }

    // print port number used
    printf("Socket binded to address using port: %d\n", port);

    // listen for connection
    printf("Creating listening queue.\n");
    if (listen(mscSocket, 5) < 0)
    {
        printf("Failed to listen.\n");
        return EXIT_FAILURE;
    }

    // listen for clients
    while (1)
    {
        if (!bsSockets.socketsAvailable)
            continue;

        socketData_t *availableClient = client_getAvailableSocket(&bsSockets);

        availableClient->socket = accept(mscSocket, (struct sockaddr *)&availableClient->addr, &availableClient->addrLen);
        printf("New Base Station accepted. ID given: %d\n", availableClient->index);

        // create thread to handle bs
        pthread_create(&availableClient->threadId, NULL, handleBs, (void *)availableClient);
        pthread_detach(availableClient->threadId);
    }

    return EXIT_SUCCESS;
}