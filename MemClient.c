#include "MemClient.h"

MemClient* memc_open()
{
    MemClient* ret = (MemClient*)malloc(sizeof(MemClient));
    /* create semaphore */
    ret->socket_lock_server = sem_open(DEFAULT_SERVER_SEMAPHORE_NAME, O_CREAT /* only open if it exists*/, 0644, 1);
    if (ret->socket_lock_server == SEM_FAILED) {
        perror("sem_open");
        exit(-1);
    }
    /* create shared memory */
    ret->shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->shm_id == -1)
    {
        perror("Check if server is running, Error creating shared memory");
        if (sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME) == -1)
            perror("sem_unlink");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->socket = shmat(ret->shm_id, (void*) 0, 0);
    if (ret->socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        return 1;
    }

    if(*(char*)(ret->socket) == MCON_UNVAIL)
    {
        printf("Error: Server not running.\n");
        /* unlink, since no server running and shared memory not needed */
        if (sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME) == -1)
            perror("sem_unlink");
        if (shmdt(ret->socket) == -1)
            perror("Error detaching shared memory");
        /* mark shared memory for removal */
        if (shmctl(ret->shm_id, IPC_RMID, NULL) == -1)
            perror("Error removing shared memory");
        free(ret);
        _Exit(-1);
    }
    return ret;
}

void memc_send(MemClient* client, void* data, _nmap_size size)
{
    // printf("MemClient.c:51 wait\n");
    for(;;)
    {
        sem_wait(client->socket_lock_server);
        if(*(char*)client->socket == MCON_EMPTY)
            break;
        sem_post(client->socket_lock_server);
    }
    // printf("MemClient.c:59 done\n");
    *((char*)client->socket) = MCON_TRANSFER;
    memcpy(client->socket+1, data, size);
    *((char*)client->socket) = MCON_WAITING;
    sem_post(client->socket_lock_server);
    // printf("MemClient.c:63 wait\n");
    sem_post(client->socket_lock_server);
    for(;;)
    {
        sem_wait(client->socket_lock_server);
        if(*(char*)client->socket == MCON_RESPONSED)
            break;
        sem_post(client->socket_lock_server);
    }
    sem_post(client->socket_lock_server);
    // printf("MemClient.c:72 done\n");
}

extern inline void memc_cleanbuf(MemClient* client)
{
    memset(client->socket + 1, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void memc_accept(MemClient* client)
{
    sem_wait(client->socket_lock_server);
    *((char*)client->socket) = MCON_EMPTY;
    sem_post(client->socket_lock_server);
}

void memc_close(MemClient* client)
{
    sem_close(client->socket_lock_server);
    /* detach shared memory */
    if (shmdt(client->socket) == -1)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }
    free(client);
}
