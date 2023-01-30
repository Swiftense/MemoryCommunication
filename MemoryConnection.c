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
        {
                            //printf("%d\n", *((char*)con->state1));
            if(*((char*)con->state1) == state_wait)
            {
                if(*((char*)con->update1) != UPDATE)
                    return;
            }
        }
        for(;w > -1; --w)
        {
                            //printf("%d\n", *((char*)con->state2));
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
    memcon_setUpdate(UPDATE,con);
    *((char*)con->state1) = state_new;
    *((char*)con->state2) = state_new;
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