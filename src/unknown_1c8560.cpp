#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "data_array.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_1c8560
{
	short field_0;
	byte field_2[2];
	long field_4;
	byte field_8[0xc];
	short field_14;
};

extern s_record_pool *g_5044c8;
void function_1e2a90(long actor_index);
void __stdcall function_b83b0(long object_index, bool unused);
void __stdcall function_b8460(long object_index, bool detach);
void __stdcall function_28e200(long arg_0);

struct s_1c8560_iterator
{
	byte *datum;
	s_record_pool_iterator records;
	long object_index;
};

PRIVATE __forceinline bool function_1c8561(s_1c8560_iterator *arg_0)
{
    arg_0->datum = NULL;
    if (g_4f55d0->active)
        arg_0->datum = data_iterator_next_inlined(&arg_0->records);
    return arg_0->datum != NULL;
}

// @retail 0x1c8560
long __stdcall function_1c8560(long arg_0, void *arg_1, long arg_2, bool *arg_3, void *arg_4, long arg_5)
{
	s_1c8560 *local_2;
	s_1c8560_iterator local_1;
	long local_0 = 0;
	if (g_4f55d0->active)
	{
		local_1.records.data = g_5044c8;
		local_1.records.index = NONE;
	}
	while (function_1c8561(&local_1))
	{
		local_2 = (s_1c8560 *)local_1.datum;
		long local_7 = local_2->field_4;
		if (local_7 != NONE)
		{
			s_actor_view *local_3 = actor_get(local_7);
			if (local_3->unknown009 || *(long *)((byte *)local_3 + 0x10) == NONE)
				continue;
		}
		local_0 += local_2->field_14;
		if (local_7 != NONE)
		{
			s_actor_view *local_4 = actor_get(local_7);
			long local_5 = local_4->unknown018;
			long local_6 = *(long *)((byte *)local_4 + 0x1c);
			function_1e2a90(local_7);
			if (local_5 != NONE)
			{
				function_b83b0(local_5, true);
				function_b8460(local_5, true);
			}
			else if (local_6 != NONE)
				function_28e200(local_6);
		}
		else
			function_28e200(local_1.records.datum_index);
	}
	*arg_3 = false;
	return local_0;
}

