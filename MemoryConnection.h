typedef void* _mem_socket;

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

#define DEFAULT_CONNECTION_BUFFER_SIZE 256
#define DEFAULT_CONNECTION_SHM "ndb_socket"
#define DEFAULT_SERVER_SEMAPHORE_NAME "ndb_ssocket_sem" /* used to block server */
#define DEFAULT_CLIENT_SEMAPHORE_NAME "ndb_csocket_sem" /* used to block all clients */
#define state1 socket-1 /* first part of state spinlock*/
#define state2 socket-2 /* second part */
#define update1 socket-3 /* state update property */

#ifndef MEMSOCKET_H
    #define MEMSOCKET_H
    enum _ConnectionState
    {
        MCON_UNVAIL,
        MCON_EMPTY,
        MCON_TRANSFER,
        MCON_WAITING,
        MCON_RESPONSED,
        MCON_ACCEPTED
    };

    enum _UpdateState
    {
        UPDATE,
        UPDATE_NONE
    };

    struct _MemCon
    {
        _mem_socket socket;
        int shm_id;
        sem_t*socket_lock_server;
    };

    extern inline void memcon_awaitAndSetState(enum _ConnectionState state_wait, enum _ConnectionState state_new, struct _MemCon* con);

    extern inline void memcon_awaitState(enum _ConnectionState state_wait, struct _MemCon* con);

    extern inline void memcon_updateState(enum _ConnectionState state_new, struct _MemCon* con);

    extern inline char memcon_setUpdate(enum _UpdateState state_new, struct _MemCon* con);

    extern inline char memcon_getState(struct _MemCon* con);
#endif