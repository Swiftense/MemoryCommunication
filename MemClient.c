#include "MemClient.h"

MemClient* memc_open()
{
    MemClient* ret = (MemClient*)malloc(sizeof(MemClient));
    /* attach semaphore */
    ret->socket_lock = sem_open(DEFAULT_SERVER_SEMAPHORE_NAME, O_CREAT /* only open if it exists*/, 0644, 1);
    if (ret->socket_lock == SEM_FAILED) {
        perror("sem_open");
        exit(-1);
    }

    /* create shared memory */
    ret->shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->shm_id <0)
    {
        perror("Check if server is running, Error creating shared memory");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->socket = shmat(ret->shm_id, (void*) 0, 0);
    if (ret->socket < (char*) 0)
    {
        perror("Error attaching shared memory");
        return 1;
    }

    ret->socket+=3;

    if(memcon_getState(ret) == MCON_UNVAIL)
    {
        printf("Error: Server not running.\n");

        if (shmdt(ret->socket) <0)
            perror("Error detaching shared memory");
        /* mark shared memory for removal */
        if (shmctl(ret->shm_id, IPC_RMID, NULL) <0)
            perror("Error removing shared memory");
        free(ret);
        _Exit(-1);
    }
    return ret;
}

void memc_send(MemClient* client, void* data, _nmap_size size)
{
    // printf("MemClient.c:50 wait\n");
    memcon_awaitAndSetState(MCON_EMPTY, MCON_TRANSFER, client);
    memcpy(client->socket, data, size);
    memcon_updateState(MCON_WAITING, client);
    sem_post(client->socket_lock);
    // printf("MemClient.c:56 wait\n");
    memcon_awaitState(MCON_RESPONSED, client);
    // printf("MemClient.c:58 done\n");
}

extern inline void memc_cleanbuf(MemClient* client)
{
    memset(client->socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void memc_accept(MemClient* client)
{
    memcon_updateState(MCON_EMPTY, client);
}

void memc_close(MemClient* client)
{
    if(sem_close(client->socket_lock) < 0)
    {
        perror("Error closing socket lock");
        _Exit(-1);
    }
    /* detach shared memory */
    if (shmdt(client->socket - 3) <0)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }
    free(client);
}

