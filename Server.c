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
        printf("Recieved Message: %s\n", memserver->socket+1);
        printf("Send answer: ");
        char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
        scanf("%[^\n]", &answer);
        mems_answer(memserver, &answer, strlen(&answer));
        exit(0);
    #elif defined(performance)
        long long l;
        long long answer = 1;
        for(;;)
        {
            mems_wait(memserver);
            l = *(long long*)(((char*)memserver->socket) + 3);
            mems_answer(memserver, &answer, 8);
            if(l % 10000 == 0)
                printf("%lld\n", l);
        }
    #else
        for(long long l;++l;)
        {
            mems_wait(memserver);
            //printf("Recieved Message: %s\n", memserver->socket+1);
            char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
            sprintf(&answer, "ser: %d", l);
            //sleep(1);
            mems_answer(memserver, &answer, strlen(&answer) +1 );
        }
    #endif
}