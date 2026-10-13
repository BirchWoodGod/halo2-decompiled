// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"

static __forceinline real slot_random(void);
static __forceinline real slot_random_range(real lower, real upper);
static __forceinline long real_to_long(real value);

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* slot type 0x24 */

short __stdcall function_1b2630(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b23c0(long actor_index, s_slot *slot);
void __stdcall function_1b2770(long actor_index, s_slot *slot);
void __stdcall function_1b2bb0(long actor_index, s_slot *slot);
bool function_26ba60(long prop_index, long actor_index, long clump_index);

// @retail 0x1b2340
short __stdcall function_1b2340(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown007)
		return 0;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (prop_node_view(node) && node->unknown24 >= 3)
			result = 3;
	}
	return result;
}

struct s_slot_24
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[0x16 - 0xd];
	short unknown16;
	short unknown18;
	byte unknown1a[0x40 - 0x1a];
};

/* the state of the actor's prop view (0 without one) */
struct s_prop_view_state
{
	short state;
};

static __forceinline short actor_prop_state(s_actor_view *actor)
{
	short state = 0;

	if (actor->prop_index != NONE)
	{
		#pragma inline_depth(0)
		s_prop_view_state *view = (s_prop_view_state *)function_25d700(actor->prop_index);
		#pragma inline_depth()

		if (view)
			state = view->state;
	}
	return state;
}

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0
{
	dword flags;
};

void *function_1e4db0(long actor_index);

// @retail 0x1b2630
short __stdcall function_1b2630(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_24 *state = (s_slot_24 *)slot;
	short result = g_46fbe8;

	if (state->unknown0c)
	{
		if (actor->prop_index != NONE)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);
			s_prop_view_fields *view = prop_node_view(node);

			if (view && function_26ba60(node->unknown08, actor_index, actor->unknown07c))
			{
				if (view->unknown70 == 0)
					function_1fb7e0(0x2e, actor_index, NULL, node->object_index, NONE);
				else
					function_1fb7e0(0x30, actor_index, NULL, node->object_index, NONE);
			}
		}
	}
	else if (actor->prop_index != NONE && prop_node_view(prop_node_get(actor->prop_index)))
	{
		return result;
	}
	return g_46fbe4;
}

s_slot_handler_2 g_47e080 ={
	{
		0x24, 2, 0, -2, 0,
		function_1b2340, function_1b2630, function_1b23c0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1b2770, 0, function_1b2bb0
};

// @retail 0x1b2bb0
void __stdcall function_1b2bb0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_slot_24 *state = (s_slot_24 *)slot;
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));
		s_character_db0 *character;

		actor->unknown4a2 = true;
		if (actor->unknown5d0)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 0;
		}
		else if (state->unknown18 - state->unknown16 < MAX(state->unknown18 / 3, real_to_long((real)g_510c54->field_2_3 * 3.0f)))
		{
			if (!view)
				goto done;
			if (view->unknown70 == 0)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 2;
			}
			else
			{
				point3f point;

				function_210850(&view->unknown78, &point);
				actor->unknown41c = 3;
				actor->unknown420 = 3;
				actor->unknown424.point = point;
			}
		}
		if (view && view->unknown70 == 0)
			actor->unknown488 = actor_prop_state(actor) >= 5;
done:
		character = (s_character_db0 *)function_1e4db0(actor_index);
		if (character)
		{
			if (character->flags & 1)
			{
				actor->unknown44a = true;
				actor->unknown449 = true;
			}
			else if ((character->flags & 2) && view && actor->unknown086 <= 3)
			{
				actor->unknown483 = true;
			}
		}
	}
}
bool function_2601f0(long actor_index, s_type_c3b527 const *point, long sector, real radius, real path_distance);
void function_25d420(long prop_ref_index, short type, long actor_index);
void function_1f86a0(long actor_index);

// @retail 0x1b23c0
bool __stdcall function_1b23c0(long actor_index, s_slot *slot)
{
    volatile bool result = true;
    s_actor_view *actor = actor_get(actor_index);
    s_prop_node_view *node = prop_node_get(actor->prop_index);
    s_prop_state_view *local_5cfb45 = prop_node_state(node);
    s_prop_view_fields *view = prop_node_view(node);
    real duration;
    if (!view)
        return false;
    if (view->unknown70 == 0)
    {
        duration = 3.0f;
        if (*(long *)((byte *)local_5cfb45 + 0x44) == NONE ||
            !function_2601f0(actor_index, &local_5cfb45->unknown48, *(long *)((byte *)local_5cfb45 + 0x44),
                actor->unknown26c != NONE ? 6.0f : 3.0f, 0.0f))
        {
            function_25d420(actor->prop_index, 3, actor_index);
            return false;
        }
    }
    else
    {
        byte *character = (byte *)function_1e4db0(actor_index);
        duration = character ? slot_random_range(*(real *)(character + 4), *(real *)(character + 8)) : 8.0f;
        if (!function_2601f0(actor_index, &view->unknown78, *(long *)((byte *)view + 0x74),
            actor->unknown26c != NONE ? 5.0f : 2.0f, 0.0f))
            return false;
    }
    s_slot_24 *state = (s_slot_24 *)slot;
    long ticks = real_to_long((real)g_510c54->field_2_3 * duration);
    state->unknown16 = (short)ticks;
    state->unknown18 = (short)ticks;
    *(short *)((byte *)state + 0x14) = 0;
    state->unknown0c = false;
    *((bool *)state + 0xd) = false;
    if (function_26ba60(node->unknown08, actor_index, actor->unknown07c))
    {
        if (view->unknown70 == 0)
        {
            byte *prop = g_50241c->data + (node->unknown08 & 0xffff) * 0xc4;
            if (!prop[0x32])
                prop[0x32] = function_1fb7e0(0x31, actor_index, NULL, node->object_index, NONE);
        }
        else
            function_1fb7e0(0x36, actor_index, NULL, node->object_index, NONE);
    }
    function_1f86a0(actor_index);
    return result;
}
