#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // used for close()
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "msc_model.h"
#include "client.h"
#include "tcp_ip_helpers.h"

const char mscIp[] = "127.0.0.1"; // using local ip only for now
int bsSocket, mscSocket = 0;
unsigned long int mscThreadId = 0;
clientSockets_t mnSockets;

void exitFunction()
{
    // do stuff to make exit graceful

    // closing msc socket if open
    if (mscSocket != 0)
    {
        printf("Closing msc socket.\n");
        shutdown(mscSocket, SHUT_RDWR);
        close(mscSocket);
    }

    // closing bs socket if open
    if (bsSocket != 0)
    {
        printf("Closing bs socket.\n");
        shutdown(bsSocket, SHUT_RDWR);
        close(bsSocket);
    }

    // close client sockets
    client_closeAllSockets(&mnSockets);

    printf("Base Station Closed.\n");
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

void *handleMsc(void *arg)
{
    char buffer[TCP_BUFFER_SIZE];
    int bytesReceived = 0;

    // receive bytes from bs socket
    while ((bytesReceived = recv(mscSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0'; // null terminate message

        // TODO: process message
    }

    // server connection lost
    printf("Lost connection to MSC.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    char BSId[MAX_ID_SIZE] = "\0";

    // check args for at least 2 arg
    if (argc < 3)
    {
        printf("Usage: ./BS <ID> <MSC port>\n");
        return EXIT_FAILURE;
    }

    // check ID len
    if (strlen(argv[1]) >= MAX_ID_SIZE - 1)
    {
        printf("Max length for ID is %d\n", MAX_ID_SIZE - 1);
        return EXIT_FAILURE;
    }

    // check port
    unsigned int mscPort = strtoul(argv[2], NULL, 10);

    // init client sockets
    client_socketsInit(&mnSockets);

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

    // create msc socket
    mscSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mscSocket < 0)
    {
        printf("Failed to create MSC socket.\n");
        return EXIT_FAILURE;
    }
    printf("MSC socket created.\n");

    // create msc addr
    struct sockaddr_in mscAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(mscIp),
        .sin_port = htons(mscPort)};

    printf("Attempting to connect to MSC (%s)... ", mscIp);
    if (connect(mscSocket, (struct sockaddr *)&mscAddr, sizeof(mscAddr)) < 0) // failed to connect
    {
        printf("Failed to connect.\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected.\n");

    // create socket
    bsSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (bsSocket < 0)
    {
        printf("Failed to create BS socket.\n");
        return EXIT_FAILURE;
    }
    printf("BS socket created.\n");

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
        bindRes = bind(bsSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

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
    if (listen(bsSocket, 5) < 0)
    {
        printf("Failed to listen.\n");
        return EXIT_FAILURE;
    }

    // create msc handler thread
    pthread_create(&mscThreadId, NULL, handleMsc, NULL);
    pthread_detach(mscThreadId);

    // listen for clients
    while (1)
    {
        if (!mnSockets.socketsAvailable)
            continue;

        socketData_t *availableClient = client_getAvailableSocket(&mnSockets);

        availableClient->socket = accept(bsSocket, (struct sockaddr *)&availableClient->addr, &availableClient->addrLen);
        printf("New Mobile Node accepted. ID given: %d\n", availableClient->index);

        // create thread to handle client
    }

    return EXIT_SUCCESS;
}