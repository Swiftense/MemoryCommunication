#include "MemClient.h"

MemClient* memc_open()
{
    MemClient* ret = (MemClient*)malloc(sizeof(MemClient));
    /* create shared memory */
    ret->con.shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->con.shm_id == -1)
    {
        perror("Check if server is running, Error creating shared memory");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->con.socket = shmat(ret->con.shm_id, (void*) 0, 0);
    if (ret->con.socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        return 1;
    }

    ret->con.socket+=3;

    if(memcon_getState(ret) == MCON_UNVAIL)
    {
        printf("Error: Server not running.\n");

        if (shmdt(ret->con.socket) == -1)
            perror("Error detaching shared memory");
        /* mark shared memory for removal */
        if (shmctl(ret->con.shm_id, IPC_RMID, NULL) == -1)
            perror("Error removing shared memory");
        free(ret);
        _Exit(-1);
    }
    return ret;
}

void memc_send(MemClient* client, void* data, _nmap_size size)
{
    client->active = CLIENT_ACTIVE;
    // printf("MemClient.c:53 wait\n");
    memcon_awaitAndSetState(MCON_EMPTY, MCON_TRANSFER, &client->con);
    memcpy(client->con.socket, data, size);
    memcon_updateState(MCON_WAITING, &client->con);
    // printf("MemClient.c:58 wait\n");
    memcon_awaitState(MCON_RESPONSED, &client->con);
    // printf("MemClient.c:60 done\n");
}

extern inline void memc_cleanbuf(MemClient* client)
{
    memset(client->con.socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void memc_accept(MemClient* client)
{
    memcon_updateState(MCON_EMPTY, &client->con);
    client->active = CLIENT_INACTIVE;
}

void memc_close(MemClient* client)
{
    if(client->active == CLIENT_ACTIVE)
        memcon_updateState(MCON_EMPTY, &client->con);
    /* detach shared memory */
    if (shmdt(client->con.socket - 3) == -1)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }
    free(client);
}

