// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A9F20.CPP: the destructor of a reference object (vtable 0x452c1c)
   whose enable/disable virtual is the library's c_a::v2 (0x2d7240), over a
   base (vtable 0x452c30) that frees itself to the allocator at 0x480118 with
   the size it keeps in its first word */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"

class c_sized_reference
{
public:
    virtual ~c_sized_reference() {}
    word flags;
    word unknown06;

    static void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_sized_reference *)block)->flags, 0x10);
    }
};

class c_reference_a9f20 : public c_sized_reference
{
public:
    virtual ~c_reference_a9f20();
};

// @retail 0xa9f20 deleting
c_reference_a9f20::~c_reference_a9f20()
{
    if (flags & 0x8000)
    {
        flags &= 0x7fff;
        /* the same object as the library's c_a, whose virtual retail calls directly */
        ((c_a *)(void *)this)->c_a::v2(false);
    }
}
