#pragma once
#include "unknown_1efac0.h"

struct c_244980_base : c_a
{
    virtual long v1(long first, long second);
    virtual void *v3();
};

struct c_marker_base : c_244980_base
{
    dword d8;
    union
    {
        dword dc;
        dword value0c;
    };
    c_marker_base(dword other)
    {
        dc = other;
        unknown06 = 1;
        d8 = 0;
    }
};

struct c_marker_object : c_marker_base
{
    dword d10;
    c_marker_object(dword value, dword other) : c_marker_base(other)
    {
        d10 = value;
    }
    void operator delete(void *block)
    {
        g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
    }
    virtual long v5() { return 2; }
    virtual void v6() { __asm { int 3 } __assume(0); }
    virtual void v7() { __asm { int 3 } __assume(0); }
    virtual void v8() { __asm { int 3 } __assume(0); }
    virtual void v9() { __asm { int 3 } __assume(0); }
    virtual void v10() { __asm { int 3 } __assume(0); }
    virtual void v11() { __asm { int 3 } __assume(0); }
    virtual void v12() { __asm { int 3 } __assume(0); }
    virtual c_marker_object *create(dword value, void *memory);
    virtual long v14(long unused) { return 0; }
};
