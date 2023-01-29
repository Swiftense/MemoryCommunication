#include "Client.h"

MemClient* memclient;

void fallback(void)
{
    printf("\nClosing Client.\n");
    memc_close(memclient);
}

void main(void)
{
    memclient = memc_open();
    printf("%d\n", *(char*)memclient->socket);
    atexit(&fallback);
    signal(SIGINT, exit);
    
    #ifdef messanger
        //sem_post(memclient->socket_lock_server);
        char message[DEFAULT_CONNECTION_BUFFER_SIZE];
        printf("Send Message: ");
        scanf("%[^\n]", &message);
        memc_send(memclient, &message, strlen(&message));
        printf("%d\n", *(char*)memclient->socket);
        printf("Recieved Answer: %s\n", memclient->socket+1);
        memc_accept(memclient);
        printf("%d\n", *(char*)memclient->socket);
    #else
        for(long long l; ++l;)
        {
            char message[DEFAULT_CONNECTION_BUFFER_SIZE];
            sprintf(&message, "cli: %lld", l);
            memc_send(memclient, &message, strlen(&message));
            printf("Recieved Answer: %s\n", memclient->socket+1);
            memc_accept(memclient);
        }
    #endif*/
}