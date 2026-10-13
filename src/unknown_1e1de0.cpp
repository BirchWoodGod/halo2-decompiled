// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E1DE0.CPP: actor helpers the ai script functions (unknown_272b70.cpp)
   call (actors.cpp; outside functions lane A needs) */

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"

/* the actors (a local view of g_4f55f0) */
struct s_actor_1e1de0
{
	byte unknown000[7];
	bool flag007;
	byte unknown008;
	bool flag009;
	byte unknown00a[0x18 - 0xa];
	long unit_index;
	long field_1c;
	byte unknown020[0x30 - 0x20];
	long squad_index;
	byte unknown034[0x306 - 0x34];
	short value306;
	byte unknown308[0x328 - 0x308];
	short value328;
	byte unknown32a[0x888 - 0x32a];
};

/* the objects (a local view) */
struct s_object_1e1de0
{
	byte unknown000[0xd4];
	long index0d4;
	byte unknown0d8[0x10a - 0xd8];
	word flags10a;
	byte unknown10c[0x134 - 0x10c];
	dword flags134;
	byte unknown138[0x2b8 - 0x138];
	real value2b8;
};

struct s_object_header_1e1de0
{
	byte unknown00[8];
	s_object_1e1de0 *object;
};

inline s_actor_1e1de0 *actor_get_1e1de0(long actor_index)
{
	return (s_actor_1e1de0 *)g_4f55f0->data + (actor_index & 0xffff);
}

inline s_object_1e1de0 *object_get_1e1de0(long object_index)
{
	return ((s_object_header_1e1de0 *)g_4e0300->data)[object_index & 0xffff].object;
}

short function_1a6fe0(long owner_index, short type);
void function_b7360(long object_index);
void function_b58c0(long index, dword mask);
void function_d0e00(long unit_index, real rate);
void __stdcall function_1e1a00(long index, long value);
void function_28e2b0(long field_1c, bool flag);
void function_203360(long squad_index);

// @retail 0x1e1de0
bool function_1e1de0(long actor_index)
{
	s_actor_1e1de0 *actor = actor_get_1e1de0(actor_index);
	bool result = true;

	if (actor->flag009 && actor->value328 > 2 && actor->value306 <= g_510c54->field_2_3 * 20)
	{
		if (function_1a6fe0(actor_index, 0x38) != NONE)
			result = false;
	}
	else
	{
		result = false;
	}
	return result;
}

// @retail 0x1e2a00
void function_1e2a00(long actor_index, bool flag, bool keep)
{
	s_actor_1e1de0 *actor = actor_get_1e1de0(actor_index);
	long squad_index = actor->squad_index;

	if (actor->flag007)
	{
		if (actor->field_1c != NONE)
			function_28e2b0(actor->field_1c, flag);
	}
	else
	{
		s_object_1e1de0 *unit = object_get_1e1de0(actor->unit_index);
		if (flag)
			unit->flags10a |= 0x40;
		else
			unit->flags10a |= 0x20;
		function_b7360(actor->unit_index);
	}

	if (!keep)
	{
		function_1e1a00(actor_index, 1);
		if (squad_index != NONE)
			function_203360(squad_index);
	}
}

// @retail 0x1e32e0
bool function_1e32e0(long actor_index, bool flag)
{
	s_actor_1e1de0 volatile const *actor = actor_get_1e1de0(actor_index);
	long unit_index = actor->unit_index;
	bool result = false;

	if (unit_index != NONE)
	{
		if (flag)
		{
			s_object_1e1de0 *unit = object_get_1e1de0(unit_index);
			unit->flags134 |= 8;
			unit->value2b8 = 0.25f;
			if (object_get_1e1de0(unit_index)->index0d4 != NONE)
				function_b58c0(object_get_1e1de0(unit_index)->index0d4, 0x800000);
		}
		else
		{
			function_d0e00(unit_index, 1.0f);
		}
		result = true;
	}
	return result;
}
