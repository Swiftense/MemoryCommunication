typedef void* _mem_socket;

#define DEFAULT_CONNECTION_BUFFER_SIZE 256
#define DEFAULT_CONNECTION_SHM "st_socket"
#define DEFAULT_SERVER_SEMAPHORE_NAME "st_ssocket_sem" /* used to block server */
#define DEFAULT_CLIENT_SEMAPHORE_NAME "st_csocket_sem" /* used to block all clients */

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
#endif