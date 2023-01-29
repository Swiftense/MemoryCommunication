#include "MemServer.h"

MemServer* mems_open()
{
    MemServer* ret = (MemServer*)malloc(sizeof(MemServer));
    /* create semaphore */
    ret->socket_lock_server = sem_open(DEFAULT_SERVER_SEMAPHORE_NAME, O_CREAT, 0644, 1);
    if (ret->socket_lock_server == SEM_FAILED) {
        perror("sem_open");
        exit(-1);
    }
    /* create shared memory */
    ret->shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, IPC_CREAT | 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->shm_id == -1)
    {
        perror("Error creating shared memory");
        return 1;
    }

    /* attach shared memory to the process's address space */
    ret->socket = shmat(ret->shm_id, (void*) 0, 0);
    if (ret->socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        return 1;
    }

    memset(ret->socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
    *((char*)ret->socket) = MCON_EMPTY;
    return ret;
}

void mems_wait(MemServer* server)
{
    //printf("MemServer.c:37 wait\n");
    for(;;)
    {
        sem_wait(server->socket_lock_server);
        if(*(char*)server->socket == MCON_WAITING)
            break;
        sem_post(server->socket_lock_server);
    }
    sem_post(server->socket_lock_server);
    //printf("MemServer.c:39 done\n");
}

extern inline void mems_cleanbuf(MemServer* server)
{
    memset(server->socket +1, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void mems_answer(MemServer* server, void* data, _nmap_size size)
{
    //printf("answering\n");
    memcpy(server->socket+1, data, size);
    sem_wait(server->socket_lock_server);
    *(char*)server->socket = MCON_RESPONSED;
    sem_post(server->socket_lock_server);
}

void mems_close(MemServer* server)
{
    *((char*)server->socket) = MCON_UNVAIL;
    sem_close(server->socket_lock_server);
    sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME);
    
    // detach shared memory from process's address space
    if (shmdt(server->socket) == -1)
    {
        perror("Error detaching shared memory");
        return 1;
    }

    // mark shared memory for removal
    if (shmctl(server->shm_id, IPC_RMID, NULL) == -1)
    {
        perror("Error removing shared memory");
        return 1;
    }
    free(server);
}
