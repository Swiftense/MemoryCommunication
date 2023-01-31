#include "MemServer.h"

MemServer* mems_open()
{
    MemServer* ret = (MemServer*)malloc(sizeof(MemServer));
    /* create shared memory */
    ret->con.shm_id = shmget(ftok(DEFAULT_CONNECTION_SHM, 0), DEFAULT_CONNECTION_BUFFER_SIZE, IPC_CREAT | 0666); // create shared memory

    /* check if shared memory was created successfully */
    if (ret->con.shm_id == -1)
    {
        perror("Error creating shared memory");
        _Exit(-1);
    }

    /* attach shared memory to the process's address space */
    ret->con.socket = shmat(ret->con.shm_id, (void*) 0, 0);
    if (ret->con.socket == (char*) -1)
    {
        perror("Error attaching shared memory");
        _Exit(-1);
    }

    memset(ret->con.socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
    ret->con.socket+=3;
    memcon_updateState(MCON_EMPTY, &ret->con);
    *((char*)ret->con.update1) = UPDATE_NONE;
    return ret;
}

void mems_wait(MemServer* server)
{
    // printf("MemServer.c:42 wait\n");
    memcon_awaitState(MCON_WAITING, &server->con);
    // printf("MemServer.c:44 done\n");
}

extern inline void mems_cleanbuf(MemServer* server)
{
    memset(server->con.socket, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void mems_answer(MemServer* server, void* data, _nmap_size size)
{
    // printf("answering\n");
    memcpy(server->con.socket, data, size);
    // printf("answered\n");
    memcon_updateState(MCON_RESPONSED, &server->con);
}

void mems_close(MemServer* server)
{
    memcon_updateState(MCON_UNVAIL, &server->con);
    
    /* detach shared memory from process's address space*/
    if (shmdt(server->con.socket-3) == -1)
    {
        perror("Error detaching shared memory");
        _Exit(-1);
    }

    /* mark shared memory for removal*/
    if (shmctl(server->con.shm_id, IPC_RMID, NULL) == -1)
    {
        perror("Error removing shared memory");
        _Exit(-1);
    }
    free(server);
}
