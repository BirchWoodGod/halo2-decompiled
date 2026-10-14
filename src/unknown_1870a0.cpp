#include "unknown_11c920.h"
#include "globals.h"
#include "network_message_types.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

class c_class_6a600;
struct s_1870a0
{
    byte field_0[0x24];
};
struct s_1870a1
{
    byte field_0[8];
    long field_8;
};
void __stdcall function_186ab0(long arg_0, real arg_1, real arg_2, s_player_action *arg_3);
void function_69040(c_class_6a600 *arg_0, s_player_action *arg_1, dword arg_2);

__forceinline real function_1870a1(real const *arg_0)
{
    union s_1870a2
    {
        real field_0;
        dword field_4;
    };
    s_1870a2 local_0;
    local_0.field_4 = *(dword volatile *)arg_0;
    return local_0.field_0;
}

#pragma inline_depth(1)
// @retail 0x1870a0
void __stdcall function_1870a0(real arg_0, real arg_1)
{
    long local_3 = NONE;
    dword local_1 = 0;
    long local_2 = 0;
    s_player_action local_0[4];
    s_player_action *local_4 = local_0;
    for (long local_5 = 0xc; local_5 < 0x1c; local_5 += 4, local_2++, local_4++)
    {
        byte *local_9 = (byte *)g_4e8c20;
        long const volatile *local_6 = (long const volatile *)(local_9 + local_5);
        local_3 = local_5 == 8 ? NONE : *local_6;
        if (local_3 != NONE)
        {
            real local_8 = function_1870a1(&arg_0);
            real local_7 = function_1870a1(&arg_1);
            function_186ab0(local_3, local_8, local_7, local_4);
            local_1 |= 1 << local_2;
        }
    }
    if (!g_4cf772 && ((s_1870a1 *)g_4cf77c)->field_8)
        function_69040((c_class_6a600 *)g_4cf77c, local_0, local_1);
}
#pragma inline_depth(255)
