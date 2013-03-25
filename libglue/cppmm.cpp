#include <stdlib.h>

void *operator new(unsigned int sz) throw ()
{
    void *p;

    if(sz == 0)
        sz = 1;
    p = (void *)malloc(sz);
    if(!p)
        abort();
    return p;
}

void *operator new[](unsigned int sz) throw ()
{
    return ::operator new(sz);
}

void operator delete(void* ptr) throw ()
{
    free(ptr);
}

void operator delete[](void *ptr) throw ()
{   
    ::operator delete(ptr);
}
