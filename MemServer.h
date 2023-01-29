#ifndef MSERVER_H
    #define MSERVER_H
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
    
    typedef struct _MemServer MemServer;
    
    struct _MemServer
    {
        _mem_socket socket;
        int shm_id;
        sem_t*socket_lock_server;
    };

    MemServer* mems_open();
    extern inline void mems_answer(MemServer* server, void* data, _nmap_size size);
    extern inline void mems_cleanbuf(MemServer* server);
    extern inline void mems_accept(MemServer* server);
    void mems_close(MemServer* server);

    #ifdef __cplusplus
        }
    #endif
#endif
