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
    ret->socket+=3;
    memcon_updateState(MCON_EMPTY, ret);
    *((char*)ret->update1) = UPDATE_NONE;
    return ret;
}

void mems_wait(MemServer* server)
{
    // printf("MemServer.c:42 wait\n");
    memcon_awaitState(MCON_WAITING, server);
    // printf("MemServer.c:44 done\n");
}

extern inline void mems_cleanbuf(MemServer* server)
{
    memset(server->socket +3, 0, DEFAULT_CONNECTION_BUFFER_SIZE);
}

extern inline void mems_answer(MemServer* server, void* data, _nmap_size size)
{
    // printf("answering\n");
    memcpy(server->socket+3, data, size);
    // printf("answered\n");
    memcon_updateState(MCON_RESPONSED, server);
}

void mems_close(MemServer* server)
{
    *((char*)server->socket) = MCON_UNVAIL;
    sem_close(server->socket_lock_server);
    sem_unlink(DEFAULT_SERVER_SEMAPHORE_NAME);
    
    // detach shared memory from process's address space
    if (shmdt(server->socket-3) == -1)
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
