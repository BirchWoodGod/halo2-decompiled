// @flags /O2 /arch:SSE /Gr
#include "object_masks_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"

struct s_13c5a0
{
	byte field_0[0xc];
	long field_c;
	long field_10;
	byte field_14[4];
	signed char field_18;
};

struct s_13c5a1
{
	byte field_0[8];
	s_13c5a0 *field_8;
};

short function_b8d30(long arg_1, long arg_2, s_object_marker *arg_3, short arg_4, bool arg_5);
void function_b9a90(long arg_1);
void __stdcall function_b8ee0(long arg_1, long arg_2, long arg_3, long arg_4);

// @retail 0x13c5a0
void function_13c5a0(long arg_1, long arg_2, long arg_3, long arg_4)
{
	s_object_marker local_1;
	if (arg_1 != NONE && function_b8d30(arg_1, arg_2, &local_1, 1, false) == 1)
	{
		long local_2 = ((s_13c5a1 *)g_4e0300->data)[arg_1 & 0xffff].field_8->field_10;
		g_4de2fc++;
		g_4de2f8 = true;
		while (local_2 != NONE)
		{
			s_13c5a0 *local_3 = ((s_13c5a1 *)g_4e0300->data)[local_2 & 0xffff].field_8;
			long local_4 = local_3->field_c;
			if (g_4de300[local_2 & 0xffff] != g_4de2fc)
			{
				g_4de300[local_2 & 0xffff] = g_4de2fc;
				if (local_3->field_18 == local_1.node_index)
				{
					function_b9a90(local_2);
					function_b8ee0(arg_1, arg_3, local_2, arg_4);
				}
			}
			local_2 = local_4;
		}
		g_4de2f8 = false;
	}
}
