#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>		/* strlen, memset */
#include <time.h>       /* time_t */


#ifndef SYNOPTIC_H
#define SYNOPTIC_H
#define PORT			   54321
#define BACKLOG            10

#define FAILURE            1
#define SUCCESS            0
#define ERROR_DUPLICATE_ENTRY 2
#define ERROR_NO_SUCH_KEY  3

#define REQUEST_LOG_FILE   "Request.log"


#define RESPONSE_OK        "OK"
#define RESPONSE_DUPLICATE "DUPLICATE"
#define RESPONSE_EMPTY     "EMPTY"

#define CLIENT_MAX_MESSAGE_SIZE 1024

typedef struct element {
    time_t addedOn;  // seconds since 00:00:00 UTC, Jan 1, 1970 (Unix timestamp)
    char key[25];    // the request received from the client
    char value[100]; // timestamp of the connection (any format)
}element_t;

#endif //SYNOPTIC_H

