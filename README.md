# MemoryCommunication
Swiftense communicates due Memory Packets to Clients on the local machine. 
#include "MemClient.h"

MemClient* memc_open()
{
    MemClient* ret = (MemClient*)malloc(sizeof(MemClient));
    /* create semaphore */
    ret->connection.socket_lock_server = sem_open(DEFAULT_SERVER_SEMAPHORE_NAME, O_CREAT /* only open if it exists*/, 0644, 1);
    if (ret->connection.socket_lock_server == SEM_FAILED) {
        perror("sem_open");
        exit(-1);
    }
    /* create shared memory */
    ret->connection.shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->connection.shm_id == -1)
    {
        perror("Check if server is running, Error creating shared memory");
        if (sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME) == -1)
            perror("sem_unlink");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->connection.socket = shmat(ret->connection.shm_id, (void*) 0, 0);
    if (ret->connection.socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        return 1;
    }

    ret->connection.socket+=6;
    if(memcon_getState(ret) == MCON_UNVAIL)
    {
        // printf("Error: Server not running.\n");
        /* unlink, since no server running and shared memory not needed */
        if (sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME) == -1)
            perror("sem_unlink");
        if (shmdt(ret->connection.socket-6) == -1)
            perror("Error detaching shared memory");
        /* mark shared memory for removal */
        if (shmctl(ret->connection.shm_id, IPC_RMID, NULL) == -1)
            perror("Error removing shared memory");
        free(ret);
        _Exit(-1);
    }
    return ret;
}

void memc_send(MemClient* client, void* data, _nmap_size size)
{
    printf("MemClient.c:52 wait\n");
    memcon_awaitAndSetState(MCON_EMPTY, MCON_TRANSFER, client);
    memcpy(client->connection.socket+1, data, size);
    memcon_updateState(MCON_WAITING, client);
    sem_post(client->connection.socket_lock_server);
    printf("MemClient.c:57 wait\n");
    memcon_awaitState(MCON_RESPONSED, client);
    printf("MemClient.c:59 done\n");
}

extern inline void memc_cleanbuf(MemClient* client)
{
    memset(client->connection.socket + 1, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void memc_accept(MemClient* client)
{
    memcon_updateState(MCON_ACCEPTED, client);
}

void memc_close(MemClient* client)
{
    sem_close(client->connection.socket_lock_server);
    /* detach shared memory */
    if (shmdt(client->connection.socket-6) == -1)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }
    free(client);
}
