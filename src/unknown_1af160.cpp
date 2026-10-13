#include "unknown_11c920.h"
#include "ai_actor.h"
#include "props.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"

// @flags /O2 /arch:SSE /Gr

#include "unknown_1af160.h"

struct s_1f4a20_entry;
struct s_1f4a20_source;
void *function_1e4be0(long arg_0);
bool __stdcall function_1ac610(long arg_0, point3f const *arg_1, s_object_marker *arg_2, short *arg_3);
bool function_1f4a20(long arg_0, s_reference arg_1, s_1f4a20_entry const *arg_2,
    bool arg_3, s_type_c3b527 *arg_4, vector3f *arg_5, signed char *arg_6, bool *arg_7, s_1f4a20_source const *arg_8);
bool function_1af530(long arg_0, long *arg_1, long *arg_2);
void function_265c30(long arg_0, long arg_1, bool arg_2);

#pragma inline_depth(1)
// @retail 0x1af160
bool function_1af160(long arg_0, s_1af160 *arg_1)
{
    long const *local_25 = &arg_0;
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (*local_25 & 0xffff) * sizeof(s_actor_view));
    s_prop_node_view *local_1 = (s_prop_node_view *)(g_502418->data + (local_0->prop_index & 0xffff) * sizeof(s_prop_node_view));
    s_type_5cfb45 *local_2 = function_25d690((s_prop_datum *)local_1);
    s_reference local_3 = local_0->unknown418;
    dword *local_4 = (dword *)function_1e4be0(arg_0);
    bool local_6 = false;
    arg_1->field_7 = local_6;
    arg_1->field_a = local_6;
    arg_1->field_b = local_6;
    arg_1->field_28 = local_6;
    arg_1->field_8 = local_6;
    volatile bool local_5 = true;
    s_object_marker local_7[32];
    short local_8[32];
    if (!*(bool *)((byte *)local_0 + 0x3c) && local_0->unknown227)
        local_6 = function_1ac610(arg_0, &local_2->position, local_7, local_8);
    s_prop_search local_9;
    memset(&local_9, 0, sizeof(local_9));
    local_9.type = 0;
    *(short *)((byte *)&local_9 + 0x69a) = 2;
    *(bool *)((byte *)&local_9 + 0x698) = true;
    local_9.unknown59 = local_4 && (*local_4 & 1);
    s_object_marker *local_10 = local_6 ? local_7 : NULL;
    *(s_object_marker **)((byte *)&local_9 + 0x64) = local_10;
    byte *local_11 = ai_scratch_buffer_get();
    s_261d20_entry local_12;
    long local_13;
    bool local_14;
    s_reference local_15 = function_261280(&local_9, arg_0, &local_12, &local_13, local_11, &local_14);
    if (REFERENCE_EQUAL(local_15, g_470fa0))
    {
        if (!local_0->unknown227)
        {
            arg_1->field_0 = 0;
            arg_1->field_6 = true;
        }
        else
        {
            ((s_actor_view *)g_4f55f0->data)[arg_0 & 0xffff].unknown040 = false;
            local_5 = false;
        }
        ai_scratch_buffer_release(local_11);
        return local_5;
    }
    bool local_17 = false;
    s_262b40_result *local_16 = function_262b40(local_15);
    bool local_18 = false;
    if (*(short *)((byte *)&local_12 + 0x5c) != 0 &&
        *(long *)((byte *)local_16 + 0x14) != NONE && *(long *)((byte *)local_16 + 0x14) != 0xffff)
    {
        local_18 = function_1f4a20(arg_0, local_15, (s_1f4a20_entry const *)&local_12,
            true, &arg_1->field_c, &arg_1->field_1c, &arg_1->field_28, &local_17, (s_1f4a20_source const *)local_10);
        if (local_18)
        {
            arg_1->field_a = true;
            arg_1->field_b = false;
        }
        else if (local_17)
            local_15 = g_470fa0;
    }
    if (!local_18 && !local_17)
        local_15 = function_2626b0(arg_0, local_15, local_13, local_11, local_14, true);
    if (!REFERENCE_EQUAL(local_15, local_3))
    {
        real local_20 = (slot_random() * 0.0f + 2.0f) * g_510c54->field_2_3;
        arg_1->field_0 = (short)real_to_long(local_20);
    }
    if (!REFERENCE_EQUAL(local_15, g_470fa0))
    {
        arg_1->field_4 = 0;
        local_0->unknown4ae = true;
        real local_21;
        real local_22;
        if (function_1af530(arg_0, (long *)&local_22, (long *)&local_21) &&
            local_21 > function_210ac0((s_type_c3b527 const *)local_16, &local_2->position))
            arg_1->field_8 = true;
    }
    long local_23 = function_1e1f20(arg_0);
    if (local_23 != NONE)
    {
        byte *local_24 = (byte *)function_1e5280(arg_0, ai_object_get(local_23)->definition_index);
        if (local_24)
        {
            bool local_26 = true;
            real local_27 = *(real *)(local_24 + 0xc);
            if (local_27 > local_1->unknown28)
                local_26 = false;
            else if (!REFERENCE_EQUAL(local_15, g_470fa0))
            {
                point3f *local_28 = &function_25d690((s_prop_datum *)local_1)->position;
                vector3f local_29;
                local_29.i = local_28->x - local_12.point.x;
                local_29.j = local_28->y - local_12.point.y;
                local_29.k = local_28->z - local_12.point.z;
                if (local_27 * local_27 > local_29.k * local_29.k + local_29.i * local_29.i + local_29.j * local_29.j)
                    local_26 = false;
            }
            function_265c30(local_0->prop_index, arg_0, local_26);
        }
    }
    ai_scratch_buffer_release(local_11);
    return local_5;
}
#pragma inline_depth(255)
