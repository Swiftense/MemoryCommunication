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
    atexit(&fallback);
    signal(SIGINT, exit);
    
    #ifdef messanger
        //sem_post(memclient->socket_lock_server);
        char message[DEFAULT_CONNECTION_BUFFER_SIZE];
        printf("Send Message: ");
        scanf("%[^\n]", &message);
        memc_send(memclient, &message, strlen(&message));
        printf("%d\n", *(char*)memclient->socket);
        printf("Recieved Answer: %s\n", memclient->socket);
        memc_accept(memclient);
        printf("%d\n", *(char*)memclient->socket);
    #elif defined(performance)
        for(unsigned long long l = 0;l < 1000001; ++l)
        {
            memc_send(memclient, &l, 8);
            memc_accept(memclient);
        }
        exit(0);
    #elif defined(count)
        for(long long l; ++l;)
        {
            char message[DEFAULT_CONNECTION_BUFFER_SIZE];
            char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
            sprintf(&message, "cli: %lld", l);
            memc_send(memclient, &message, strlen(&message)+1);
            //strcpy(&answer, memclient->socket);
            //sleep(1);
            printf("Recieved Answer: %s\n", memclient->socket);
            memc_accept(memclient);
            printf("Sent message: %s\n", &message);
            //printf("Recieved Answer: %s\n", &answer);
        }
    #else
        printf("Usage: ./client.sh count\n       ./client.sh performance\n       ./client.sh messanger");
    #endif
}