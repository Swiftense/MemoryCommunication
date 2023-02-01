#include "MemServer.h"

MemServer* mems_open()
{
    MemServer* ret = (MemServer*)malloc(sizeof(MemServer));
    /* attach semaphore */
    ret->socket_lock = sem_open(DEFAULT_SERVER_SEMAPHORE_NAME, O_CREAT /* only open if it exists*/, 0644, 0);
    if (ret->socket_lock == SEM_FAILED) {
        perror("sem_open");
        exit(-1);
    }

    /* create shared memory */
    ret->shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, IPC_CREAT | 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->shm_id < 0)
    {
        perror("Error creating shared memory");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->socket = shmat(ret->shm_id, (void*) 0, 0);
    if (ret->socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        _Exit(-1);
    }

    memset(ret->socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
    ret->socket+=3;
    memcon_updateState(MCON_EMPTY, ret);
    *((char*)ret->update1) = UPDATE_NONE;
    return ret;
}

void mems_wait(MemServer* server)
{
    // printf("MemServer.c:42 wait\n");
    sem_wait(server->socket_lock);
    memcon_awaitState(MCON_WAITING, server);
    // printf("MemServer.c:44 done\n");
}

extern inline void mems_cleanbuf(MemServer* server)
{
    memset(server->socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void mems_answer(MemServer* server, void* data, _nmap_size size)
{
    //printf("answering\n");
    memcpy(server->socket, data, size);
    //printf("answered\n");
    memcon_updateState(MCON_RESPONSED, server);
}

void mems_close(MemServer* server)
{
    memcon_updateState(MCON_UNVAIL, server);
    if(sem_close(server->socket_lock) < 0)
    {
        perror("Error closing socket lock");
        _Exit(-1);
    }
    if(sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME) < 0)
    {
        perror("Error closing unlinking socket lock");
        _Exit(-1);
    } 
    
    /* detach shared memory from process's address space*/
    if (shmdt(server->socket-3) < 0)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }

    /* mark shared memory for removal*/
    if (shmctl(server->shm_id, IPC_RMID, NULL) < 0)
    {
        perror("Error removing shared memory");
        _Exit(-1);
    }
    free(server);
}
