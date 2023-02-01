#ifndef MSERVER_H
    #define MSERVER_H
    #ifdef __cplusplus
        extern "C" {
    #endif

    #include <sys/shm.h>

    #include <unistd.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>

    #include "MemoryConnection.h"
    #include "null.h"
    #include "NMap_types.h"
    
    typedef struct _MemCon MemServer;

    MemServer* mems_open();
    extern inline void mems_answer(MemServer* server, void* data, _nmap_size size);
    extern inline void mems_cleanbuf(MemServer* server);
    void mems_close(MemServer* server);

    #ifdef __cplusplus
        }
    #endif
#endif
