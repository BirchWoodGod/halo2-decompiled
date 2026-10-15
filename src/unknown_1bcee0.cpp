// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include <string.h>
#include "unknown_2605d0.h"

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
bool __stdcall function_1bcfd0(long actor_index, s_slot *slot);
bool __stdcall function_1bd230(long actor_index, s_slot *slot);
short __stdcall function_1bd350(long actor_index, short level, bool active);
bool __stdcall function_1f4280(long actor_index);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1bcee0
bool __stdcall function_1bcee0(long actor_index, s_slot *slot)
{
	s_unit_request request;

	memset(&request, 0, sizeof(request));
	long unit_index = actor_get(actor_index)->unknown018;
	request.type = 0x2c;
	function_e6900(unit_index, &request);
	((s_slot_6c *)slot)->unknown0c = 0;
	s_actor_view *actor = actor_get(actor_index);
	actor->unknown50c = false;
	actor->unknown5ac = NONE;
	actor->unknown5b0 = NONE;
	actor->unknown5b4 = 0;
	actor->unknown5b6 = 0;
	_ReadWriteBarrier();
	actor->unknown4ac = 0;
	actor->unknown504 = 0;
	return true;
}

#pragma function(_ReadWriteBarrier)

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
	(t_slot_proc)function_1bcfd0, 0, function_1bd1e0
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


void __stdcall function_2628f0(long arg_0, s_reference arg_1);
bool __stdcall function_1f8a70(long arg_0, long arg_1);

struct s_1bcfd2
{
	short field_0;
	bool field_2;
	byte field_3;
	real field_4;
	real field_8;
	s_type_c3b527 field_c;
	long field_1c;
	real field_20;
	real field_24;
	bool field_28;
	bool field_29;
	byte field_2a[0x38 - 0x2a];
	long field_38;
	bool field_3c;
};

PRIVATE __forceinline void function_1bcfd2(s_1bcfd2 *arg_0)
{
	arg_0->field_0 = 0;
	arg_0->field_2 = false;
	arg_0->field_4 = 0.0f;
	arg_0->field_8 = 0.0f;
	arg_0->field_20 = 0.0f;
	arg_0->field_24 = 0.0f;
	arg_0->field_28 = false;
	arg_0->field_29 = false;
	arg_0->field_38 = NONE;
	arg_0->field_3c = false;
}

PRIVATE __forceinline void function_1bcfd1(long arg_0, s_type_c3b527 const *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (!local_0->unknown229)
		function_1f86a0(arg_0);
	else
	{
		function_2628f0(arg_0, g_470fa0);
		if (local_0->unknown4ac == 2 && local_0->unknown4c8 == NONE &&
			!(function_210a30(arg_1, &local_0->unknown4b8) > 0.1f * 0.1f))
		{
			if (local_0->unknown040 && !local_0->unknown506)
				function_1f8a70(arg_0, 0);
		}
		else
		{
			function_1f86a0(arg_0);
			function_1bcfd2((s_1bcfd2 *)&local_0->unknown4ac);
			local_0->unknown4b8 = *arg_1;
			local_0->unknown4ac = 2;
			local_0->unknown4ae = true;
			local_0->unknown4c8 = NONE;
			local_0->unknown4e4 = NONE;
			if (local_0->unknown229)
				local_0->unknown656 = 0;
			function_1f8a70(arg_0, 0);
		}
	}
}

// @retail 0x1bcfd0
bool __stdcall function_1bcfd0(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (!local_0->unknown50c || local_0->unknown504 != 1)
	{
		s_2605d0_request local_1;
		s_261d20_entry local_2[512];
		memset(&local_1, 0, sizeof(local_1));
		local_1.unknown056 = true;
		short local_3 = function_261d20(arg_0, local_2, 512, &local_1);
		if (local_3 > 0)
		{
			s_slot_6c *local_4 = (s_slot_6c *)arg_1;
			if (local_4->unknown0c >= local_3)
				local_4->unknown0c = 0;
			point3f local_5;
			s_type_c3b527 local_6;
			function_210850(*(s_type_c3b527 const **)local_2[local_4->unknown0c].unknown00, &local_5);
			local_6.point = local_5;
			local_6.output_index = NONE;
			function_1bcfd1(arg_0, &local_6);
		}
	}
	return true;
}
