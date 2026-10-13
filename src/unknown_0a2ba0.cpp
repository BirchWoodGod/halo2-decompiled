// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0A2BA0.CPP: the fire event of a weapon (built with SSE, apart from
   its neighbours in unknown_09b910.cpp) */

#include "unit_requests.h"
#include "effects.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0a58d0.h"
#include "object_types_21_2.h"
#include <string.h>
#include <xmmintrin.h>

struct s_z_event_header
{
    word identifier;
    byte flags, type;
    dword unknown04;
    byte *object;
};

void function_d0930(long unit_index, vector3f *direction);
void __stdcall function_102fb0(long weapon_index, short barrel);
void function_103e60(long weapon_index, short barrel);
void __stdcall function_fd980(long weapon_index);
transform4x3f *function_b8c00(long object_index, long *count);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *result);
struct s_object_relevance_source
{
    long object_index, identifier;
    byte unknown08[0x1c - 8];
    real first, second;
};
struct s_object_relevance_result
{
    real first, second;
    long object_index, identifier;
};
void function_82b30(s_object_relevance_result const *source, s_object_relevance_source *result);

// @retail 0xa2ba0
bool c_weapon_fire_event::v11(long a, long const *entities, long c, void const *data)
{
    volatile bool result = false;
    long entity = function_a58d0(entities[0]);
    if (entity == NONE) return result;
    s_z_event_header *headers = (s_z_event_header *)g_4e0300->data;
    s_z_event_header *header = headers + (entity & 0xffff);
    byte const *event = (byte const *)data;
    long unit = NONE;
    volatile long weapon = c;
    if (header->type == 2) weapon = entity;
    else
    {
        long slot = *(long const *)event;
        if (!((1 << header->type) & 3) || slot < 0 || slot >= 4 || (header->object[0x10a] & 4)) return result;
        weapon = *(long *)(header->object + 0x218 + slot * 4);
        unit = *(long *)(header->object + 0x24c);
        if (unit == NONE) unit = entity;
    }
    if (weapon == NONE) return result;
    byte *weapon_object = headers[weapon & 0xffff].object;
    long tag = *(long *)weapon_object;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    long barrel = *(long const *)(event + 8);
    if (*(long const *)(event + 4) != tag || barrel < 0 || barrel >= *(long *)(definition + 0x2d0)) return result;
    if (unit != NONE)
    {
        byte *unit_object = headers[unit & 0xffff].object;
        s_object_relevance_source source;
        source.object_index = source.identifier = *(long *)source.unknown08 = NONE;
        *(word *)(source.unknown08 + 0x10) = 0;
        source.first = source.second = 0.0f;
        function_82b30((s_object_relevance_result const *)(event + 0x28), &source);
        if ((source.first > 0.0f || source.second > 0.0f) && source.object_index != NONE)
            memcpy(unit_object + 0x1c8, &source, sizeof(source));
        byte *barrel_definition = *(byte **)(definition + 0x2d4) + barrel * 0xec;
        if ((*(dword *)barrel_definition & 0x2000) && event[0x38])
        {
            long target = function_a58d0(*(long const *)(event + 0x3c));
            long attachment = *(long const *)(event + 0x40);
            if (target != NONE && attachment != NONE)
            {
                weapon_object[0x177] = (byte)attachment;
                *(long *)(weapon_object + 0x194) = target;
            }
        }
        if (event[0xc] && entities[1] != NONE)
        {
            long target = function_a58d0(entities[1]);
            if (target != NONE)
            {
                long count = 0;
                transform4x3f *matrices = function_b8c00(target, &count);
                short node = *(short const *)(event + 0xe);
                if (node >= 0 && node < count)
                {
                    transform4x3f_apply_point(matrices + node, (point3f const *)(event + 0x10),
                        (point3f *)(unit_object + 0x1d4));
                    unit_object[0x1e0] |= 2;
                }
            }
        }
        function_d0930(unit, (vector3f *)(event + 0x1c));
    }
    function_102fb0(weapon, (short)barrel);
    function_103e60(weapon, (short)barrel);
    function_fd980(weapon);
    if (unit != NONE && event[0xc]) headers[unit & 0xffff].object[0x1e0] &= ~2;
    result = true;
    return result;
}
