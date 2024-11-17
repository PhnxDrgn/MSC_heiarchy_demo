#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // used for close()
#include <stdbool.h>
#include <signal.h>
#include "client.h"
#include "tcp_ip_helpers.h"

#define MAX_ID_SIZE 128

int serverSocket = 0;
clientSockets_t clientSockets;

void exitFunction()
{
    // do stuff to make exit graceful

    // closing server socket if open
    if (serverSocket != 0)
    {
        printf("Closing server socket.\n");
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
    }

    // close client sockets
    client_closeSockets(&clientSockets);

    printf("BS Closed.\n");
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

int main(int argc, char *argv[])
{
    char BSId[MAX_ID_SIZE] = "\0";

    // check args for at least 1 arg
    if (argc < 2)
    {
        printf("Usage: ./BS <ID>\n");
        return EXIT_FAILURE;
    }

    // check ID len
    if (strlen(argv[1]) >= MAX_ID_SIZE - 1)
    {
        printf("Max length for ID is %d\n", MAX_ID_SIZE - 1);
        return EXIT_FAILURE;
    }

    // init client sockets
    client_socketsInit(&clientSockets);

    // connect function to handle exits
    atexit(exitFunction);

    // close sockets on segmentation fault
    signal(SIGSEGV, signalHandler);

    // close sockets on abort signal
    signal(SIGABRT, signalHandler);

    // close sockets on interactive attention
    signal(SIGINT, signalHandler);

    // store ID
    strncpy(BSId, argv[1], sizeof(BSId));

    // print hello message
    printf("Starting Base Station with ID: %s\n", BSId);

    // create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
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
        bindRes = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

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
    if (listen(serverSocket, 5) < 0)
    {
        printf("Failed to listen.\n");
        return EXIT_FAILURE;
    }

    // listen for clients
    while (1)
    {
        if (!clientSockets.socketsAvailable)
            continue;

        socketData_t *availableClient = client_getAvailableSocket(&clientSockets);

        availableClient->socket = accept(serverSocket, (struct sockaddr *)&availableClient->addr, &availableClient->addrLen);
        printf("New Mobile Node accepted. ID given: %d\n", availableClient->index);

        // create thread to handle client
    }

    return EXIT_SUCCESS;
}