#include <pthread.h>
#include <signal.h>
#include "hastable.h"
#include "synoptic.h"

/* Thread routine to serve connection to client. */
void *pthread_routine(void *arg);

/* Signal handler to handle SIGTERM and SIGINT signals. */
void signal_handler(int signal_number);

void SetupSignalHandler();

int CreateServerSocket(int port);

int server_socket_fd;

hashtable_t *hashtable = NULL;

pthread_mutex_t database_mutex;
pthread_mutex_t file_mutex;
FILE *fpLogFile = NULL;

int main(int argc, char *argv[])
{
    int port, new_socket_fd;
    pthread_attr_t pthread_client_attr;
    pthread_t pthread;
    socklen_t client_address_len;
    struct sockaddr_in client_address;

    /* Get port from command line arguments or stdin.
     * For this server, this is fixed to 1113*/
    port = PORT;

    /*Create the server socket */
    server_socket_fd = CreateServerSocket(port);

    /*Setup the signal handler*/
    SetupSignalHandler();

    /*Initialize the mutex*/
    pthread_mutex_init(&database_mutex, NULL);
    pthread_mutex_init(&file_mutex, NULL);

    /*Setup the log file*/
    fpLogFile = fopen(REQUEST_LOG_FILE,"wb");
    if (!fpLogFile)
    {
        printf("Failed to create the log file\n");
        printf("Server Exiting\n");
        exit(FAILURE);
    }

    /*Setup hash table to store the database values*/
    hashtable = ht_create( 65536 );

    /* Initialise pthread attribute to create detached threads. */
    if (pthread_attr_init(&pthread_client_attr) != 0) {
        perror("pthread_attr_init");
        exit(FAILURE);
    }
    if (pthread_attr_setdetachstate(&pthread_client_attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        exit(FAILURE);
    }

    while (1) {
        /* Accept connection to client. */
        client_address_len = sizeof (client_address);
        new_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket_fd == -1) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");
        unsigned int *thread_arg = (unsigned int *) malloc(sizeof(unsigned int));
        *thread_arg = new_socket_fd;
        /* Create thread to serve connection to client. */
        if (pthread_create(&pthread, &pthread_client_attr, pthread_routine, (void *)thread_arg) != 0) {
            perror("pthread_create");
            continue;
        }
    }

    return 0;
}


int CreateServerSocket(int port)
{
    struct sockaddr_in address;
    int socket_fd;

    /* Initialise IPv4 address. */
    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    /* Create TCP socket. */
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* Bind address to socket. */
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof (address)) == -1) {
        perror("bind");
        exit(1);
    }

    /* Listen on socket. */
    if (listen(socket_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // Configure server socket
    int enable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    return socket_fd;
}

void SetupSignalHandler() {/* Assign signal handlers to signals. */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
}

int server_process_request(char *readMsg, size_t readlen, char**response, size_t* response_len )
{
    int status = FAILURE;
    if (!readMsg || !response || !response_len)
    {
        printf("Incorrect Parameters\n");
        return FAILURE;
    }

    char *key = strtok(readMsg,":");
    char *value = strtok(NULL,":");

    if (key && value)
    {
        entry_t *databaseEntry = (entry_t *)malloc(sizeof(entry_t));
        if (databaseEntry)
        {
            memset(databaseEntry,0x00, sizeof (entry_t));
            databaseEntry->element.addedOn = time(NULL);
            strncpy(databaseEntry->element.key,key,
                    strlen(key) + 1 > sizeof(databaseEntry->element.key) ? sizeof(databaseEntry->element.key) : strlen(key) + 1);
            strncpy(databaseEntry->element.value,value,
                    strlen(value) + 1 > sizeof(databaseEntry->element.value) ? sizeof(databaseEntry->element.value) : strlen(value) + 1);
            pthread_mutex_lock(&database_mutex);
            status = ht_set(hashtable, databaseEntry);
            pthread_mutex_unlock(&database_mutex);
            if(status == SUCCESS)
            {
                *response = malloc(sizeof(char) * sizeof(RESPONSE_OK));
                memcpy(*response,RESPONSE_OK,sizeof(RESPONSE_OK));
                *response_len = sizeof(RESPONSE_OK);
            }
            else if (status == ERROR_DUPLICATE_ENTRY)
            {
                *response = malloc(sizeof(char) * sizeof(RESPONSE_DUPLICATE));
                memcpy(*response,RESPONSE_DUPLICATE,sizeof(RESPONSE_DUPLICATE));
                *response_len = sizeof(RESPONSE_DUPLICATE);
            }

        }
    }
    if (key && !value)
    {
        pthread_mutex_lock(&database_mutex);
        entry_t *databaseEntry = ht_get(hashtable, key);
        pthread_mutex_unlock(&database_mutex);
        if (databaseEntry)
        {
            *response = malloc(sizeof(char) * sizeof(databaseEntry->element.value));
            memcpy(*response,databaseEntry->element.value,sizeof(databaseEntry->element.value));
            *response_len = sizeof(databaseEntry->element.value);
            status = SUCCESS;

        } else {
            *response = malloc(sizeof(char) * sizeof(RESPONSE_EMPTY));
            memcpy(*response,RESPONSE_EMPTY,sizeof(RESPONSE_EMPTY));
            *response_len = sizeof(RESPONSE_EMPTY);
            status = ERROR_NO_SUCH_KEY;
        }
    }

    time_t ticks = time(NULL);
    char *statusString = (status == SUCCESS) ? "SUCCESS": (status == ERROR_DUPLICATE_ENTRY) ? "ERROR_DUPLICATE_ENTRY" : (status == ERROR_NO_SUCH_KEY) ? "ERROR_NO_SUCH_KEY" : "FAILURE";
    pthread_mutex_lock(&file_mutex);
    fprintf(fpLogFile, "%.24s\r\n", ctime(&ticks));
    fprintf(fpLogFile,"%s ->%s:%s\n",statusString,key,value);
    pthread_mutex_unlock(&file_mutex);
    return status;
}

void *pthread_routine(void *arg)
{
    int client_socket = *(int*) arg;
    free(arg);
    char readMsg[CLIENT_MAX_MESSAGE_SIZE] = {0};

    bool isClientConnected =  true;
    while(isClientConnected)
    {
        char *response = NULL;
        size_t response_len = 0u;

        size_t readlen = read(client_socket,readMsg, CLIENT_MAX_MESSAGE_SIZE);
        if (readlen == 0) {
            printf("Client disconnected; closing server connection");
            isClientConnected = false;
        } else if (readlen < 0){
            perror("error in read");
            continue;
        } else{
            readMsg[strcspn(readMsg, "\r\n")] = 0;
            server_process_request(readMsg,readlen, &response, &response_len );
        }

        if (response_len)
        {
            write(client_socket,response,response_len);
            //free(response);
            response = NULL;
        }
    }

    close(client_socket);
    return NULL;
}

void signal_handler(int signal_number)
{
    close(server_socket_fd);
    fclose(fpLogFile);
    exit(0);
}
