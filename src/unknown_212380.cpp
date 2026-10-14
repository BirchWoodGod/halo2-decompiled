#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_19d220.h"
#include <string.h>
#include <wchar.h>

// @flags /O2 /Ob1 /Gr

long function_215c00(long arg_0, wchar_t const *arg_1);
void __stdcall function_215e60(long arg_0);
void function_1a0180(long arg_0, long arg_1, word *arg_2);
bool function_19d650(s_game_variant *arg_0);
bool function_212ec0(s_game_variant *arg_0, long arg_1);

PRIVATE __forceinline void function_2123b0(s_game_variant *arg_0, long arg_1)
{
    memset(arg_0, 0, sizeof(*arg_0));
    wchar_t local_0[0x100];
    byte *local_1 = (byte *)g_4e034c;
    local_0[0] = 0;
    if (local_1)
    {
        long local_2 = *(long *)(local_1 + 0x16c);
        if (local_2 != NONE)
        {
            byte *local_3 = g_4e3b44[local_2 & 0xffff].bytes;
            long local_4 = *(long *)(*(byte **)(local_3 + 4) + 0x1c);
            if (local_4 != NONE)
                function_1a0180(local_4, arg_1, (word *)local_0);
        }
    }
    wcsncpy(arg_0->name, local_0, 0x1f);
}

// @retail 0x212380
long function_212380(long arg_0, long arg_1, byte *arg_2)
{
    long const *local_0 = &arg_1;
    byte *const *local_1 = &arg_2;
    long local_2 = function_215c00(arg_0, (wchar_t const *)*local_1);
    if (local_2 != NONE)
    {
        union
        {
            s_game_variant field_0;
            unsigned __int64 field_8[0x130 / 8];
        } local_3;
        byte *local_4 = (byte *)&local_3.field_0;
        switch (arg_0)
        {
        case 1:
            function_2123b0(&local_3.field_0, 0xe000780);
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9a) | 0xf9a;
            *(long *)(local_4 + 0x44) = 2;
            *(long *)(local_4 + 0x50) = 0x19;
            *(long *)(local_4 + 0x74) = 0x10;
            *(long *)(local_4 + 0x78) = 0x10;
            *(long *)(local_4 + 0x80) = 5;
            *(long *)(local_4 + 0x84) = 5;
            *(long *)(local_4 + 0xa8) = 2;
            *(long *)(local_4 + 0xac) = 0xa;
            *(dword *)(local_4 + 0xf0) = (*(dword *)(local_4 + 0xf0) & 0xfffffffa) | 2;
            break;
        case 2:
            function_2123b0(&local_3.field_0, 0xc000782);
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9a) | 0xf9a;
            *(long *)(local_4 + 0x44) = 4;
            *(long *)(local_4 + 0x50) = 0x78;
            *(long *)(local_4 + 0x80) = 0x10;
            *(long *)(local_4 + 0x84) = 0x10;
            *(long *)(local_4 + 0x8c) = 5;
            *(long *)(local_4 + 0x90) = 5;
            *(long *)(local_4 + 0xac) = 0xa;
            *(dword *)(local_4 + 0xf0) &= 0xffffffe0;
            *(short *)(local_4 + 0xf4) = 0x3c;
            *(long *)(local_4 + 0xa8) = 2;
            break;
        case 4:
            function_2123b0(&local_3.field_0, 0xf000781);
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9a) | 0xf9a;
            *(long *)(local_4 + 0x44) = 3;
            *(long *)(local_4 + 0x50) = 0x78;
            *(long *)(local_4 + 0x80) = 0x10;
            *(long *)(local_4 + 0x84) = 0x10;
            *(long *)(local_4 + 0x8c) = 5;
            *(long *)(local_4 + 0x90) = 5;
            *(long *)(local_4 + 0xac) = 0xa;
            *(dword *)(local_4 + 0xf0) &= 0xfffffff8;
            *(short *)(local_4 + 0xf4) = 1;
            *(short *)(local_4 + 0xf6) = 0;
            *(short *)(local_4 + 0xf8) = 0;
            *(short *)(local_4 + 0xfa) = 0;
            *(long *)(local_4 + 0xa8) = 2;
            break;
        case 5:
            function_2123b0(&local_3.field_0, 0x12000783);
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9a) | 0xf9a;
            *(long *)(local_4 + 0x44) = 7;
            *(long *)(local_4 + 0x50) = 0xf;
            *(long *)(local_4 + 0x74) = 0x10;
            *(long *)(local_4 + 0x78) = 0x10;
            *(long *)(local_4 + 0x80) = 5;
            *(long *)(local_4 + 0x84) = 5;
            *(long *)(local_4 + 0xac) = 0xa;
            *(long *)(local_4 + 0xb4) = 2;
            *(short *)(local_4 + 0x100) = 2;
            *(dword *)(local_4 + 0xf0) = (*(dword *)(local_4 + 0xf0) & 0xffffffdb) | 0x1b;
            break;
        case 7:
            function_2123b0(&local_3.field_0, 0xb00077f);
            *(dword *)(local_4 + 0x48) |= 1;
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9b) | 0xf9a;
            *(long *)(local_4 + 0x44) = 1;
            *(long *)(local_4 + 0x50) = 3;
            *(long *)(local_4 + 0x74) = 0x10;
            *(long *)(local_4 + 0x78) = 0x10;
            *(long *)(local_4 + 0x80) = 0xa;
            *(long *)(local_4 + 0x84) = 0xa;
            *(long *)(local_4 + 0xa8) = 2;
            *(long *)(local_4 + 0xac) = 0xa;
            *(dword *)(local_4 + 0xf0) = (*(dword *)(local_4 + 0xf0) & 0xffffff03) | 2;
            *(long *)(local_4 + 0xfc) = 0;
            *(short *)(local_4 + 0x108) = 0;
            *(short *)(local_4 + 0x10a) = 0;
            *(long *)(local_4 + 0xf4) = 0x1e;
            *(long *)(local_4 + 0xf8) = 0;
            *(long *)(local_4 + 0x104) = 0;
            *(long *)(local_4 + 0x100) = 0;
            break;
        case 8:
            function_2123b0(&local_3.field_0, 0xf000785);
            *(dword *)(local_4 + 0x48) |= 1;
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9b) | 0xf9a;
            *(long *)(local_4 + 0x44) = 9;
            *(long *)(local_4 + 0x50) = 3;
            *(long *)(local_4 + 0x74) = 0x10;
            *(long *)(local_4 + 0x78) = 0x10;
            *(long *)(local_4 + 0x80) = 0xa;
            *(long *)(local_4 + 0x84) = 0xa;
            *(long *)(local_4 + 0xac) = 0xa;
            *(dword *)(local_4 + 0xf0) = (*(dword *)(local_4 + 0xf0) & 0xffffff23) | 0x22;
            *(long *)(local_4 + 0xfc) = 0;
            *(short *)(local_4 + 0x108) = 5;
            *(short *)(local_4 + 0x10a) = 3;
            *(long *)(local_4 + 0xf4) = 0x1e;
            *(long *)(local_4 + 0xf8) = 0;
            *(long *)(local_4 + 0x104) = 0;
            *(long *)(local_4 + 0x100) = 1;
            *(long *)(local_4 + 0xa8) = 2;
            break;
        case 9:
            function_2123b0(&local_3.field_0, 0x13000784);
            *(dword *)(local_4 + 0x48) |= 1;
            *(dword *)(local_4 + 0x48) = (*(dword *)(local_4 + 0x48) & 0xffff8f9b) | 0xf9a;
            *(long *)(local_4 + 0x44) = 8;
            *(long *)(local_4 + 0x50) = 0x12c;
            *(long *)(local_4 + 0x74) = 0x10;
            *(long *)(local_4 + 0x78) = 0x10;
            *(long *)(local_4 + 0x80) = 0xa;
            *(long *)(local_4 + 0x84) = 0xa;
            *(long *)(local_4 + 0xac) = 0xa;
            *(short *)(local_4 + 0xf4) = 5;
            *(short *)(local_4 + 0xf0) = 3;
            *(short *)(local_4 + 0xf2) = 5;
            *(long *)(local_4 + 0xa8) = 2;
            break;
        default:
            __assume(0);
        }
        *(short *)(local_4 + 0x42) = 0;
        *(long *)(local_4 + 0x4c) = 0;
        *(long *)(local_4 + 0x58) = 0;
        *(long *)(local_4 + 0x7c) = 0;
        *(long *)(local_4 + 0x88) = 0;
        *(long *)(local_4 + 0xa4) = 0;
        *(long *)(local_4 + 0xb0) = 0;
        local_4[0xcc] = 0;
        local_4[0xcd] = 0;
        local_4[0xce] = 0;
        local_4[0xcf] = 0;
        local_4[0xd0] = 0;
        local_4[0xd1] = 0;
        local_4[0xd2] = 0;
        local_4[0xd3] = 0;
        local_4[0xd4] = 0;
        local_4[0xd5] = 0;
        local_4[0xd6] = 0;
        local_4[0xd7] = 0;
        local_4[3] = 0xff;
        *(long *)(local_4 + 0x54) = 0x1e0;
        union
        {
            s_game_variant field_0;
            unsigned __int64 field_8[0x130 / 8];
        } local_6;
        local_6.field_0 = local_3.field_0;
        ((byte *)&local_6.field_0)[0] &= 0xfe;
        function_19d650(&local_6.field_0);
        wcsncpy(local_6.field_0.name, (wchar_t const *)*local_1, 0x1f);
        local_6.field_0.name[0x1f] = 0;
        if (!function_212ec0(&local_6.field_0, local_2))
        {
            function_215e60(local_2);
            local_2 = NONE;
        }
    }
    return local_2;
}
