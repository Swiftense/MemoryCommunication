#include "Server.h"

MemServer* memserver;

void fallback(void)
{
    printf("\nClosing Server.\n");
    mems_close(memserver);
}

void main(void)
{
    atexit(&fallback);
    signal(SIGINT, exit);
    // another process can open the named pipe for reading
    memserver = mems_open();
    #ifdef messanger
        mems_wait(memserver);
        printf("Recieved Message: %s\n", memserver->socket);
        printf("Send answer: ");
        char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
        scanf("%[^\n]", &answer);
        mems_answer(memserver, &answer, strlen(&answer)+1);
        exit(0);
    #elif defined(performance)
        long long l;
        long long answer = 1;
        for(;;)
        {
            mems_wait(memserver);
            l = *(long long*)(((char*)memserver->socket));
            mems_answer(memserver, &answer, 8);
            if(l % 10000 == 0)
                printf("%lld\n", l);
        }
    #else
        char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
        for(long long l;++l;)
        {
            mems_wait(memserver);
            printf("Recieved Message: %s\n", memserver->socket);
            sprintf(&answer, "ser: %d", l);
            mems_answer(memserver, &answer, strlen(&answer)+1);
        }
    #endif
}