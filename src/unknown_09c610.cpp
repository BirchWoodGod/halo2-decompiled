#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "event_definitions.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

__declspec(noinline) void *function_122c10(long arg_0, long arg_1);
void function_db760(long arg_0, long arg_1, long arg_2, long arg_3);
struct s_18444a;
void __stdcall function_184440(long arg_0, long arg_1, s_18444a const *arg_2, long arg_3);
extern byte *g_4ed280;

static __forceinline long function_9c912(long arg_0, long arg_1, long arg_2)
{
    long local_0;
    if (arg_0 < arg_1)
        local_0 = arg_1;
    else
    {
        local_0 = arg_2;
        if (arg_0 <= arg_2)
            local_0 = arg_0;
    }
    return local_0;
}

// @retail 0x9c910
bool c_damage_section_response_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool local_0 = false;
    arg_1 = (long const *)function_a58d0(arg_1[0]);
    volatile bool local_12 = local_0;
    if ((long)arg_1 != NONE)
    {
        byte *local_3 = (byte *)((s_object_header *)g_4e0300->data)[(long)arg_1 & 0xffff].object;
        byte *local_4 = g_4e3b44[*(long *)local_3 & 0xffff].bytes;
        long local_5 = *(long *)(local_4 + 0x38);
        if (local_5 != NONE)
        {
            byte *local_6 = local_3 + *(short *)(local_3 + 0x122);
            long const *local_2 = (long const *)arg_3;
            long local_7 = local_2[0];
            if (local_7 >= 0 && local_7 < (long)((dword)(*(short *)(local_3 + 0x120)) / 8))
            {
                byte *local_8 = g_4e3b44[local_5 & 0xffff].bytes;
                if (*(long *)(local_8 + 0x60) > 0)
                {
                    byte *local_9 = *(byte **)(*(byte **)(local_8 + 0x64) + 0xc0) + local_7 * 0x38;
                    arg_3 = (void const *)local_2[1];
                    long local_10 = (long)arg_3;
                    if (local_10 >= 0 && local_10 < *(long *)(local_9 + 0xc))
                    {
                        long local_11 = local_2[2];
                        if (function_9c912(local_11, 0, 1) == local_11)
                        {
                            long local_14 = *(volatile long const *)(local_2 + 1);
                            if (!(*(word *)(local_6 + local_7 * 8) & (1 << local_14)))
                            {
                                if (local_11 != 1 || *(long *)(*(byte **)(local_9 + 0x10) + local_14 * 0x50 + 0x34) != NONE)
                                {
                                    function_db760(local_7, *(volatile long *)&arg_3, *(volatile long *)&arg_1, local_11);
                                    local_0 = true;
                                }
                                else
                                    local_0 = local_12;
                            }
                            else
                                local_0 = local_12;
                        }
                    }
                }
            }
        }
    }
    return local_0;
}

struct s_9cca0
{
    long field_0;
    byte field_4[0x18];
    long field_1c;
    long field_20;
    byte field_24[0xc];
    vector3f field_30;
    vector3f field_3c;
    byte field_48[0x34];
    short field_7c;
    byte field_7e[10];
};

#pragma inline_depth(0)
// @retail 0x9cca0
bool c_breakable_surface_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    volatile bool local_2 = false;
    s_9cca0 local_1;
    byte const *local_0 = (byte const *)arg_3;
    if (function_122c10(0x6a707421, *(long const *)(local_0 + 0x30)))
    {
        memset((byte *)&local_1 + 4, 0, 0x78);
        memset(local_1.field_7e, 0, sizeof(local_1.field_7e));
        local_1.field_1c = *(long const *)(local_0 + 0xc);
        local_1.field_20 = *(long const *)(local_0 + 0x10);
        local_1.field_30 = *(vector3f const *)(local_0 + 0x24);
        local_1.field_3c = *(vector3f const *)(local_0 + 0x18);
        local_1.field_7c = -1;
        local_1.field_0 = *(long const *)(local_0 + 0x30);
        if (*g_4ed280 && local_1.field_0 != NONE)
            function_184440(*(long const *)local_0, *(long const *)(local_0 + 4), (s_18444a const *)&local_1, *(long const *)(local_0 + 8));
    }
    return false;
}
#pragma inline_depth(255)

struct s_9f880
{
    long field_0;
    long field_4;
    long field_8;
    long field_c;
    short field_10;
    byte field_12[2];
    real field_14;
    byte field_18;
};
struct s_9f881
{
    byte field_0[0x10a];
    struct s_9f882
    {
        word field_0 : 1;
        word field_1 : 1;
        word field_2 : 1;
        word field_3 : 13;
    } field_10a;
};

struct s_unit_melee_hit;
void __stdcall function_cfc90(long arg_0, long arg_1, s_unit_melee_hit const *arg_2);
bool function_183f50(long arg_0, long arg_1, long arg_2);
extern short g_47d8e0;

// @retail 0x9f880
bool c_unit_melee_damage_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    long local_1 = function_a58d0(arg_1[1]);
    byte const *local_2 = (byte const *)arg_3;
    long local_3 = *(long const *)(local_2 + 4);
    long local_4 = *(long const *)(local_2 + 8);
    long local_5 = *(long const *)(local_2 + 0xc);
    if (local_3 != NONE && !function_183f50(local_4, local_5, local_3))
    {
        local_3 = NONE;
        local_4 = NONE;
        local_5 = NONE;
    }
    byte local_6 = local_2[0x18];
    if ((local_6 & 0x3f) >= 0x2a)
        local_6 &= 0xc0;
    short local_7 = *(short const *)(local_2 + 0x10);
    byte *local_8 = 0;
    if (local_7 != -1 && local_7 >= 0 && local_7 < *(long *)((byte *)g_4e034c + 0x150))
        local_8 = *(byte **)((byte *)g_4e034c + 0x154) + local_7 * 0xb4;
    if (!local_8)
        local_7 = g_47d8e0;
    if (local_0 != NONE)
    {
        byte *local_9 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
        if (local_9[0xaa] == 0 && !TEST_FIELD_BIT(((s_9f881 *)local_9)->field_10a.field_2) &&
            (local_1 != NONE || local_3 != NONE) && function_122c10(0x6a707421, *(long const *)local_2))
        {
            s_9f880 local_10;
            local_10.field_0 = local_1;
            local_10.field_4 = local_3;
            local_10.field_8 = local_4;
            local_10.field_c = local_5;
            local_10.field_10 = local_7;
            local_10.field_14 = *(real const *)(local_2 + 0x14);
            local_10.field_18 = local_6;
            function_cfc90(local_0, *(long const *)local_2, (s_unit_melee_hit const *)&local_10);
        }
    }
    return false;
}

struct s_damage_report;
point3f *function_b9dd0(long arg_0, point3f *arg_1);
void function_d6a70(long arg_0);
void function_d9640(s_damage_report const *arg_0, long arg_1);
void __stdcall function_e4a20(point3f const *arg_0, long arg_1, long arg_2, bool arg_3);

struct s_9c610
{
    byte field_0;
    byte field_1[3];
    dword field_4;
    long field_8;
    long field_c;
    long field_10;
    short field_14;
    byte field_16[2];
    vector3f field_18;
    point3f field_24;
    real field_30;
    real field_34;
    real field_38;
    long field_3c;
    short field_40;
    byte field_42[2];
    real field_44;
    real field_48;
    real field_4c;
    dword field_50;
};

#pragma inline_depth(0)
// @retail 0x9c610
bool c_damage_aftermath_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    bool local_0 = false;
    long local_1 = function_a58d0(arg_1[0]);
    long local_2 = function_a58d0(arg_1[1]);
    if (local_1 != NONE)
    {
        byte const *local_3 = (byte const *)arg_3;
        long local_4 = NONE;
        long &local_5 = *(long *)&arg_1;
        local_5 = NONE;
        short local_6 = *(short const *)(local_3 + 8);
        if (local_6 != -1)
        {
            long local_7 = local_6;
            if (local_7 != NONE && local_7 >= 0 && local_7 < g_4e8c24->high_water_index)
            {
                byte *local_8 = g_4e8c24->data + local_7 * g_4e8c24->size;
                short local_9 = *(short *)local_8;
                if (local_9)
                {
                    local_4 = ((long)local_9 << 16) | local_7;
                    local_5 = *(char *)(local_8 + 0xc0);
                }
            }
        }
        if (function_122c10(0x6a707421, *(long const *)local_3))
        {
            short local_11 = *(short volatile *)&local_5;
            s_9c610 local_10;
            memset(&local_10, 0, sizeof(local_10));
            local_10.field_8 = *(long const *)local_3;
            local_10.field_10 = local_1;
            local_10.field_c = local_4;
            local_10.field_14 = local_11;
            if (local_3[0xa])
                local_10.field_18 = *(vector3f const *)(local_3 + 0xc);
            local_10.field_4 = *(dword const *)(local_3 + 0x20);
            local_10.field_34 = *(real const *)(local_3 + 0x18);
            local_10.field_38 = *(real const *)(local_3 + 0x1c);
            local_10.field_3c = *(short const *)(local_3 + 0x2c);
            local_10.field_40 = *(short const *)(local_3 + 0x2e);
            local_10.field_44 = *(real const *)(local_3 + 0x28);
            local_10.field_48 = *(real const *)(local_3 + 0x24);
            local_10.field_50 = *(dword const *)(local_3 + 0x30);
            local_10.field_0 = local_3[0x34];
            local_10.field_30 = 0.0f;
            function_b9dd0(local_2, &local_10.field_24);
            local_10.field_4c = 0.0f;
            if (local_3[0x20] & 2)
                function_d6a70(local_2);
            function_d9640((s_damage_report const *)&local_10, local_2);
            if (g_4e0300->data[(local_2 & 0xffff) * 12 + 3] == 0 && (*(char const *)(local_3 + 0x20) < 0))
                function_e4a20((point3f const *)(local_3 + 0xc), *(short const *)(local_3 + 0x2e), local_2, false);
        }
        local_0 = true;
    }
    return local_0;
}
#pragma inline_depth(255)

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
void __stdcall function_191fab(long arg_0, long arg_1);
void __stdcall function_191e51(long arg_0, long arg_1);
void __stdcall function_191ec4(long arg_0, long arg_1, short arg_2);
void function_f8160(long arg_0);
long function_1896c0(real arg_0, long arg_1);
void player_speed_request_14ea20(long arg_0);
void player_speed_request_14eb10(long arg_0);
void player_speed_request_14ec00(long arg_0);
void function_19ca10(long arg_0);

// @retail 0x9ffa0
bool c_unit_pickup_event_definition::v11(long arg_0, long const *arg_1, long arg_2, void const *arg_3)
{
    long local_0 = function_a58d0(arg_1[0]);
    if (local_0 != NONE)
    {
        byte *local_1 = (byte *)function_badc0(local_0, 3);
        if (local_1 && *(long *)(local_1 + 0x13c) != NONE)
        {
            local_1 = (byte *)function_badc0(local_0, 3);
            long local_2 = NONE;
            if (local_1)
                local_2 = *(long *)(local_1 + 0x13c);
            long local_3 = *(short *)(g_4e8c24->data + (local_2 & 0xffff) * 0x21c + 0x28);
            if (local_3 != NONE)
            {
                byte const *local_4 = (byte const *)arg_3;
                                if (*(short const *)local_4 == 4 || (*(long const *)(local_4 + 4) != NONE && function_122c10(0x6974656d, *(long const *)(local_4 + 4))))
                {
                    switch (*(short const *)local_4)
                    {
                    case 0:
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    case 1:
                    {
                        byte *local_7 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        if ((1 << local_7[0]) & 4)
                        {
                            long local_8 = *(long *)(local_7 + 0x268);
                            if (local_8 != NONE)
                                function_1896c0(1.0f, local_8);
                        }
                        else
                            function_f8160(*(long const *)(local_4 + 4));
                        function_191ec4(*(long const *)(local_4 + 4), local_3, *(short const *)(local_4 + 8));
                        break;
                    }
                    case 2:
                        function_191e51(*(long const *)(local_4 + 4), local_3);
                        function_f8160(*(long const *)(local_4 + 4));
                        break;
                    case 3:
                    {
                        byte *local_9 = g_4e3b44[*(long const *)(local_4 + 4) & 0xffff].bytes;
                        short local_10 = *(short *)(local_9 + 0x12c);
                        byte *local_11 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_12 = *(long *)(local_11 + 0x13c);
                        if (local_10 == 2)
                            player_speed_request_14ea20(local_12);
                        else if (local_10 == 5)
                            player_speed_request_14ec00(local_12);
                        else if (local_10 == 3)
                            player_speed_request_14eb10(local_12);
                        function_f8160(*(long const *)(local_4 + 4));
                        function_191fab(*(long const *)(local_4 + 4), local_3);
                        break;
                    }
                    case 4:
                    {
                        byte *local_13 = (byte *)((s_object_header *)g_4e0300->data)[local_0 & 0xffff].object;
                        long local_14 = *(long *)(local_13 + 0x13c);
                        if (local_14 != NONE)
                            function_19ca10(local_14);
                        break;
                    }
                    }
                }
            }
        }
    }
    return false;
}
