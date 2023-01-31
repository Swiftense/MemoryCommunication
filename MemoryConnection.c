#include "MemoryConnection.h"

extern inline void memcon_awaitAndSetState(enum _ConnectionState state_wait, enum _ConnectionState state_new, struct _MemCon* con)
{
    for(int w = 0;;)
    {
        for(;w < 5; ++w)
        {
            if(*((char*)con->state1) == state_wait)
            {
                if(*((char*)con->update1) == UPDATE)
                {
                    continue;
                }
                memcon_updateState(state_new, con);
                return;
            }
        }
        for(;w > -1; --w)
        {
            if(*((char*)con->state2) == state_wait)
            {
                if(*((char*)con->update1) == UPDATE)
                {
                    continue;
                }
                memcon_updateState(state_new, con);   
                return;
            }
        }
    }
}

extern inline void memcon_awaitState(enum _ConnectionState state_wait, struct _MemCon* con)
{
    for(int w = 0;;)
    {
        for(;w < 5; ++w)
        {;
            if(*((char*)con->state1) == state_wait)
            {
                if(*((char*)con->update1) != UPDATE)
                    return;
            }
        }
        for(;w > -1; --w)
        {
            if(*((char*)con->state2) == state_wait)
            {
                if(*((char*)con->update1) != UPDATE)
                    return;
            }
        }
    }
}

extern inline void memcon_updateState(enum _ConnectionState state_new, struct _MemCon* con)
{
    // critical change, didnt found a fix except for waiting
    memcon_setUpdate(UPDATE,con);
    // usleep(10000);
    *((char*)con->state1) = state_new;
    *((char*)con->state2) = state_new;
    // usleep(10000);
    memcon_setUpdate(UPDATE_NONE,con);
}

extern inline char memcon_getState(struct _MemCon* con)
{
    return *((char*)con->state1);
}

extern inline char memcon_setUpdate(enum _UpdateState state_new, struct _MemCon* con)
{
    *((char*)con->update1) = state_new;
}