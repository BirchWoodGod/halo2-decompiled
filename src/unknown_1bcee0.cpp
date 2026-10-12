// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include <string.h>

/* slot type 0x6c and slot group 7 */

struct s_slot_6c
{
	s_slot_header header;
	short unknown0c;
	byte unknown0e[0x40 - 0xe];
};

void function_1f86a0(long index);
void *function_1e5030(long actor_index);

/* the block of the actor's character tag function_1e5030 returns */
struct s_character_5030
{
	real lower;
	real upper;
};

bool __stdcall function_1bcee0(long actor_index, s_slot *slot);
void __stdcall function_1bcfd0(long actor_index, s_slot *slot);
bool __stdcall function_1bd230(long actor_index, s_slot *slot);
short __stdcall function_1bd350(long actor_index, short level, bool active);
void __stdcall function_1f4280(long actor_index);

// @retail 0x1bcee0
bool __stdcall function_1bcee0(long actor_index, s_slot *slot)
{
	s_unit_request request;

	memset(&request, 0, sizeof(request));
	long unit_index = actor_get(actor_index)->unknown018;
	request.type = 0x2c;
	function_e6900(unit_index, &request);
	((s_slot_6c *)slot)->unknown0c = 0;
	actor_reset_state(actor_index);
	return true;
}

// @retail 0x1bcf90
short __stdcall function_1bcf90(long actor_index, s_slot *slot, bool active)
{
	if (actor_get(actor_index)->unknown504 == 2)
	{
		s_slot_6c *state = (s_slot_6c *)slot;

		function_1f86a0(actor_index);
		state->unknown0c++;
	}
	return g_46fbe8;
}

// @retail 0x1bd1e0
void __stdcall function_1bd1e0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 4;
	actor->unknown420 = 4;
	actor->unknown424.vector = *g_4687a8;
}

// @retail 0x1bd330
short __stdcall function_1bd330(long actor_index, s_slot *slot, bool active)
{
	s_slot_6c *state = (s_slot_6c *)slot;
	short result = g_46fbe8;

	if (--state->unknown0c <= 0)
		result = g_46fbe4;
	return result;
}

// @retail 0x1bd350
short __stdcall function_1bd350(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = function_1a79e0(actor_index, level, active);

	function_1f4280(actor_index);
	if (actor->prop_index != NONE)
	{
		actor->unknown41c = 4;
		actor->unknown420 = 2;
	}
	return result;
}

s_slot_handler_2 g_47eba8 =
{
	{
		0x6c, 2, NONE, -2, 0,
		function_1adcd0, function_1bcf90, function_1bcee0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1bcfd0, 0, function_1bd1e0
};

/* the children of slot group 7 */
s_slot_child g_46fba8[1] =
{
	{2, 0, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47ebf8 =
{
	{
		7, 1, 0, -2, 0,
		function_1adcd0, function_1bd330, function_1bd230, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1bd350, 1, g_46fba8
};

// @retail 0x1bd230
bool __stdcall function_1bd230(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	s_slot_6c *state = (s_slot_6c *)slot;

	if (actor->unknown328 < 12)
	{
		s_character_5030 *character = (s_character_5030 *)function_1e5030(actor_index);

		if (character && character->upper > 0.0f)
		{
			real seconds = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, character->lower, character->upper) * g_510c54->field_2_3;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}

			state->unknown0c = (short)ticks;
			if (state->unknown0c > 0)
			{
				if (actor->unknown328 >= 10)
					state->unknown0c /= 2;
				result = true;
			}
		}
	}

	return result;
}
