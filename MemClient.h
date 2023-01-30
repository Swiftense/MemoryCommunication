#ifndef MCLIENT_H
    #define MCLIENT_H
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
    
    typedef struct _MemCon MemClient;

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
