#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ID_SIZE 128

void exitFunction()
{
    // do stuff to make exit graceful
    printf("MSC Closed.\n");
}

int main(int argc, char *argv[])
{
    char mscId[MAX_ID_SIZE] = "\0";

    // check args for at least 1 arg
    if (argc < 2)
    {
        printf("Minumum of 1 arg required for the MSC ID\n");
        return EXIT_FAILURE;
    }

    // check ID len
    if (strlen(argv[1]) >= MAX_ID_SIZE - 1)
    {
        printf("Max length for ID is %d\n", MAX_ID_SIZE - 1);
        return EXIT_FAILURE;
    }

    // connect function to handle exits
    atexit(exitFunction);

    // store ID
    strncpy(mscId, argv[1], sizeof(mscId));

    // print hello message
    printf("Starting Mobile Switching Center with ID: %s\n", mscId);

    return EXIT_SUCCESS;
}