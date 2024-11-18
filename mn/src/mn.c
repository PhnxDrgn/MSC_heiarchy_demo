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
#include "tcp_ip_helpers.h"

const char bsIp[] = "127.0.0.1"; // using local ip only for now
int bsSocket = 0;
unsigned long int bsThreadId = 0;

void exitFunction()
{
    // do stuff to make exit graceful

    // closing bs socket if open
    if (bsSocket != 0)
    {
        printf("Closing bs socket.\n");
        shutdown(bsSocket, SHUT_RDWR);
        close(bsSocket);
        bsSocket = 0;
    }

    printf("Mobile Node Closed.\n");
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

void *handleMsc()
{
    char buffer[TCP_BUFFER_SIZE];
    int bytesReceived = 0;

    // receive bytes from bs socket
    while ((bytesReceived = recv(bsSocket, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0'; // null terminate message

        // TODO: process message
    }

    // server connection lost
    printf("Lost connection to BS.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    char BSId[MAX_ID_SIZE] = "\0";

    // check args for at least 2 arg
    if (argc < 3)
    {
        printf("Usage: ./MN <ID> <BS port>\n");
        return EXIT_FAILURE;
    }

    // check ID len
    if (strlen(argv[1]) >= MAX_ID_SIZE - 1)
    {
        printf("Max length for ID is %d\n", MAX_ID_SIZE - 1);
        return EXIT_FAILURE;
    }

    // check port
    unsigned int bsPort = strtoul(argv[2], NULL, 10);
    if (bsPort == 0)
    {
        printf("Invalid port argument.\n");
        return EXIT_FAILURE;
    }

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
    printf("Starting Mobile Node with ID: %s\n", BSId);

    // create bs socket
    bsSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (bsSocket < 0)
    {
        printf("Failed to create BS socket.\n");
        return EXIT_FAILURE;
    }
    printf("BS socket created.\n");

    // create bs addr
    struct sockaddr_in bsAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(bsIp),
        .sin_port = htons(bsPort)};

    printf("Attempting to connect to BS (%s)... ", bsIp);
    if (connect(bsSocket, (struct sockaddr *)&bsAddr, sizeof(bsAddr)) < 0) // failed to connect
    {
        printf("Failed to connect.\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected.\n");

    handleMsc();

    return EXIT_SUCCESS;
}