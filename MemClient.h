#ifndef MCLIENT_H
    #define MCLIENT_H
    #ifdef __cplusplus
        extern "C" {
    #endif

    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <sys/ioctl.h>
    #include <sys/shm.h>

    #include <semaphore.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>

    #include "MemoryConnection.h"
    #include "null.h"
    #include "NMap_types.h"
    
    typedef struct _MemClient MemClient;
    
    struct _MemClient
    {
        _mem_socket socket;
        int shm_id;
        sem_t*socket_lock_server;
    };

    MemClient* memc_open();
    /* waits until a message is sent to socket 
       and awaits response of Server */
    void memc_send(MemClient* client, void* data, _nmap_size size);
    extern inline void memc_cleanbuf(MemClient* client);
    extern inline void memc_accept(MemClient* client);
    void memc_close(MemClient* client);

    #ifdef __cplusplus
        }
    #endif
#endif
