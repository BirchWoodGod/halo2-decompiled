// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_10ACA0.CPP: the object list of g_5107f4 (unknown_10a980.cpp) and
   an object's fade */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_10a980.h"
#include "unknown_10aca0.h"

/* g_5107f4: 32 entries of 0x14 bytes, then the object of each */

struct s_10a980_entry
{
	long name;
	byte unknown04[0x14 - 4];
};

struct s_10a980_view
{
	s_10a980_entry entries[32];
	long object_indices[32];
};

/* the objects (a local view) */
struct s_object_10aca0
{
	byte unknown000[0xc0];
	word : 9;
	word flag_c0_9 : 1;
	word : 6;
	byte unknown0c2[0xf4 - 0xc2];
	real fade_value;
	byte unknown0f8[4];
	real fade_rate;
	byte unknown100[8];
	char fade_ticks;
	byte unknown109;
	word : 11;
	word fading : 1;
	word : 4;
};

struct s_object_header_10aca0
{
	byte unknown00[8];
	s_object_10aca0 *object;
};

inline s_object_10aca0 *object_get_10aca0(long object_index)
{
	return ((s_object_header_10aca0 *)g_4e0300->data)[object_index & 0xffff].object;
}

// @retail 0x10aca0
void function_10aca0(long object_index, long name)
{
	if (object_index != NONE && name && name != NONE)
	{
		s_10a980_view *globals = (s_10a980_view *)g_5107f4;
		for (long index = 0; index < 32; index++)
		{
			if (globals->object_indices[index] == object_index && globals->entries[index].name == name)
				globals->object_indices[index] = NONE;
		}
	}
}

// @retail 0x10ace0
void function_10ace0(long object_index)
{
	if (object_index != NONE)
	{
		s_object_10aca0 *object = object_get_10aca0(object_index);
		if (TEST_FIELD_BIT(object->flag_c0_9))
		{
			s_10a980_view *globals = (s_10a980_view *)g_5107f4;
			for (long index = 0; index < 32; index++)
			{
				if (globals->object_indices[index] == object_index)
					globals->object_indices[index] = NONE;
			}
			object->flag_c0_9 = false;
		}
	}
}

// @retail 0x10ad90
void function_10ad90(long object_index, real target, real seconds)
{
	if (object_index != NONE)
	{
		s_object_10aca0 *object = object_get_10aca0(object_index);
		if (target == 0.0f && seconds == 0.0f)
		{
			object->fading = false;
			object->fade_value = 0.0f;
			object->fade_rate = 0.0f;
			object->fade_ticks = NONE;
		}
		else
		{
			real ticks_real = (real)g_510c54->field_2_3 * seconds;
			long ticks;
			__asm
			{
				fld ticks_real
				fistp ticks
			}
			if (ticks > 0x7f)
				ticks = 0x7f;
			object->fade_ticks = (char)ticks;
			if (object->fade_ticks == 0)
			{
				real *value = &object->fade_value;
				*value = target;
				real *rate = &object->fade_rate;
				*(volatile real *)rate = 0.0f;
			}
			else
			{
				real *rate = &object->fade_rate;
				*rate = (target - object->fade_value) / object->fade_ticks;
			}
			object->fading = true;
		}
	}
}