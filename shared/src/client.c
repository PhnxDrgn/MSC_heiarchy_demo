#include "client.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void client_socketsInit(clientSockets_t *clientSockets)
{
    for (unsigned int ii = 0; ii < MAX_NUM_CLIENTS; ii++)
    {
        clientSockets->clients[ii].index = ii;
        clientSockets->clients[ii].socket = 0;
        clientSockets->clients[ii].addrLen = sizeof(struct sockaddr_in);
        clientSockets->clients[ii].available = true;
        memset(&clientSockets->clients[ii].addr, 0, sizeof(struct sockaddr_in));
    }

    clientSockets->socketsAvailable = true;
}

void client_closeSockets(clientSockets_t *clientSockets)
{
    for (unsigned int ii = 0; ii < MAX_NUM_CLIENTS; ii++)
    {
        if (clientSockets->clients[ii].socket > 0)
        {
            printf("Closing client %d\n", clientSockets->clients[ii].index);
            shutdown(clientSockets->clients[ii].socket, SHUT_RDWR);
            close(clientSockets->clients[ii].socket);
        }

        // reset values
        clientSockets->clients[ii].socket = 0;
        memset(&clientSockets->clients[ii].addr, 0, sizeof(struct sockaddr_in));
        clientSockets->clients[ii].available = true;
    }

    if (!clientSockets->socketsAvailable)
        printf("Client socket available.\n");

    clientSockets->socketsAvailable = true;
}

socketData_t *client_getAvailableSocket(clientSockets_t *clientSockets)
{
    for (unsigned int ii = 0; ii < MAX_NUM_CLIENTS; ii++)
    {
        if (clientSockets->clients[ii].available)
        {
            clientSockets->clients[ii].available = false;
            return &clientSockets->clients[ii];
        }
    }

    // none found in for loop
    clientSockets->socketsAvailable = false;
    printf("Out of client sockets.\n");
    return NULL;
}