// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"

/* slot group 4 */

struct s_slot_04
{
	s_slot_header header;
	byte unknown0c[5];
	bool unknown11;
	bool unknown12;
	byte unknown13[0x40 - 0x13];
};

void __stdcall function_1b2e00(long actor_index, s_slot *slot);
short __stdcall function_1b2e80(long actor_index, short level, long active);
void function_258b20(long index, long actor_index);

short function_258a00(long actor_index, long unknown, long cs_index, short *result);

// @retail 0x1b2e80
short __stdcall function_1b2e80(long actor_index, short level, long active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 2;
	s_slot_04 *state = level < 4 ? (s_slot_04 *)&actor->slots[level] : NULL;
	if (state->unknown12)
	{
		s_slot *child = level + 1 < 4 ? &actor->slots[level + 1] : NULL;
		short type = child->type;
		if (type >= 0 && type < k_slot_type_count &&
			g_46eeb8[type]->unknown8 != g_46f348 &&
			(g_46eeb8[type]->mask & g_4ee4ec) == g_4ee4ec &&
			(g_557c40[type >> 5] & (1 << (type & 31))) && child->state == 0 &&
			g_46eeb8[type]->evaluate(actor_index, child, *(bool *)&active) == g_46fbe8)
		{
			result = g_46fbe8;
		}
		else
			state->unknown12 = false;
	}
	if (!state->unknown12)
	{
		short status = function_258a00(actor_index, active, actor->unknown85c, &result);
		if (status == 1 || status == 2)
		{
			state->unknown11 = true;
			result = 2;
		}
		else if (*(short const volatile *)&result != 2)
			state->unknown12 = true;
	}
	if (result != 0 && *(long *)actor->unknown01c != NONE)
	{
		actor->unknown4a4 = 7;
		actor->unknown4a8 = NONE;
		actor->unknown4a5 = 0;
	}
	return result;
}

// @retail 0x1b2da0
short __stdcall function_1b2da0(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->unknown85c != NONE)
		result = 3;
	return result;
}

// @retail 0x1b2de0
bool __stdcall function_1b2de0(long actor_index, s_slot *slot)
{
	s_slot_04 *state = (s_slot_04 *)slot;

	state->unknown12 = false;
	state->unknown11 = false;
	return true;
}

/* Keep the call to 0x258b20; the lookup stays in this body. */
#pragma inline_depth(0)
// @retail 0x1b2e00
void __stdcall function_1b2e00(long actor_index, s_slot *slot)
{
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_view));
	s_slot_04 *state = (s_slot_04 *)slot;

	if (!state->unknown11 && actor->unknown85c != NONE)
		function_258b20(actor->unknown85c, actor_index);
}
#pragma inline_depth(255)

// @retail 0x1b2e40
short __stdcall function_1b2e40(long actor_index, s_slot *slot, bool active)
{
	s_slot_04 *state = (s_slot_04 *)slot;
	short result = g_46fbe8;

	if (actor_get(actor_index)->unknown85c == NONE)
	{
		state->unknown11 = true;
		result = g_46fbe4;
	}
	return result;
}

s_slot_handler_1 g_47e170 =
{
	{
		4, 1, NONE, -2, 0,
		function_1b2da0, function_1b2e40, function_1b2de0, function_1b2e00, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_choose)function_1b2e80, 0, 0
};
