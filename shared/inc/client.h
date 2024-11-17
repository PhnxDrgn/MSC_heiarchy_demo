#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define MAX_NUM_CLIENTS 32

typedef struct
{
    int socket;
    struct sockaddr_in addr;
    bool available;
    socklen_t addrLen;
    unsigned int index;
} socketData_t;

typedef struct
{
    bool socketsAvailable;
    socketData_t clients[MAX_NUM_CLIENTS];
} clientSockets_t;

/**
 * Initializes clientScockets
 */
void client_socketsInit(clientSockets_t *clientSockets);

/**
 * Function to close the client socket and updat the available flag
 */
void client_closeSockets(clientSockets_t *clientSockets);

/**
 * Seaches clientSockets_t to find available socketData_t pointer
 */
socketData_t *client_getAvailableSocket(clientSockets_t *clientSockets);

#endif // CLIENT_H