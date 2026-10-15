// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"

/* slot type 0x3a's last update: the actor moves to the slot's reference
   until it has one it can follow, or gives it up when its unit has fought
   too long or the player's team is its enemy */

struct s_slot_3a_view
{
	s_slot_header header;
	byte unknown0c[3];
	bool unknown0f;
	s_reference reference;
	bool unknown14;
	byte unknown15[0x40 - 0x15];
};

bool __stdcall function_1f4280(long actor_index);
bool __stdcall function_1f46f0(long actor_index, short type, s_reference reference, byte *scratch, bool unknown);
void __stdcall function_1b0110(long actor_index, s_slot *slot);

// @retail 0x1b0110
void __stdcall function_1b0110(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_3a_view *state = (s_slot_3a_view *)slot;

	actor->unknown41c = 4;
	actor->unknown420 = 0;
	actor->unknown489[1] = true;
	actor->unknown44b[0] = true;
	actor->unknown449 = true;
	actor->unknown446[2] = true;
	actor->unknown481 = true;
	s_reference reference = state->reference;

	if (REFERENCE_EQUAL(reference, g_470fa0))
	{
		function_1f4280(actor_index);
	}
	else if (actor->unknown040)
	{
		if (function_1f46f0(actor_index, 4, reference, NULL, false))
		{
			function_2628f0(actor_index, state->reference);
			actor->unknown3f0 = state->unknown14;
		}
		else if (actor->unknown5b4 > 4 || team_is_enemy(actor->unknown024, 1))
		{
			if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0))
			{
				function_262800(actor_index, actor->unknown418, false);
				function_1f4280(actor_index);
				function_2628f0(actor_index, g_470fa0);
			}
			state->reference = g_470fa0;
			state->unknown0f = true;
		}
	}
}
