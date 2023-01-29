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
    #else
        for(long long l;++l;)
        {
            mems_wait(memserver);
            printf("Recieved Message: %s\n", memserver->socket+1);
            mems_cleanbuf(memserver);
            char answer[DEFAULT_CONNECTION_BUFFER_SIZE];
            sprintf(&answer, "ser: %d", l);
            sleep(1);
            mems_answer(memserver, &answer, strlen(&answer));
        }
    #endif
}