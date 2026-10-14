// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_138C10.CPP: recomputes the game options' cluster masks (+0x1138,
   +0x1178, +0x11b8) and tells the subsystems which masks changed */

#include "unknown_11c920.h"
#include "unknown_440dd8.h"
#include "globals.h"
#include <string.h>

struct s_138c10
{
	byte field_0[0x58];
	dword *field_58;
	byte field_5c[0x9c - 0x5c];
	long field_9c;
};

void function_138b30(dword *arg_1, bool arg_2, long arg_3);
bool function_1c9b50(dword *mask, short count);
void function_13e3e0(dword const *a, dword const *b, dword *destination, long bit_count);
short function_138ab0();

#define CLUSTER_MASK(options, offset) ((dword *)((byte *)(options) + (offset)))

#pragma inline_depth(0)
// @retail 0x138c10
void function_138c10(void)
{
	s_138c10 *bsp = (s_138c10 *)g_4e0348;
	dword old_1138[16];
	dword mask[16];
	dword old_11b8[16];
	memcpy(old_1138, CLUSTER_MASK(g_4e6948, 0x1138), sizeof(old_1138));
	memcpy(old_11b8, CLUSTER_MASK(g_4e6948, 0x11b8), sizeof(old_11b8));
	function_138b30(CLUSTER_MASK(g_4e6948, 0x1138), false, 0);
	function_138b30(CLUSTER_MASK(g_4e6948, 0x1178), true, 0);
	if (function_1c9b50(mask, (short)bsp->field_9c))
	{
		function_13e3e0(CLUSTER_MASK(g_4e6948, 0x1138), mask, CLUSTER_MASK(g_4e6948, 0x11b8), bsp->field_9c);
	}
	else
	{
		memcpy(CLUSTER_MASK(g_4e6948, 0x11b8), CLUSTER_MASK(g_4e6948, 0x1138), ((bsp->field_9c + 31) >> 5) * sizeof(dword));
	}
	short cluster = function_138ab0();
	if (cluster != NONE)
	{
		dword *visible = CLUSTER_MASK(g_4e6948, 0x11b8);
		function_13e3e0(visible, bsp->field_58 + ((bsp->field_9c + 31) >> 5) * cluster, visible, bsp->field_9c);
	}
	if (memcmp(old_1138, CLUSTER_MASK(g_4e6948, 0x1138), sizeof(old_1138)))
	{
		for (dword i = 0; i < 68; i++)
		{
			if (g_440dd8[i].field_1c)
				g_440dd8[i].field_1c(old_1138, CLUSTER_MASK(g_4e6948, 0x1138), bsp->field_9c);
		}
	}
	if (memcmp(old_11b8, CLUSTER_MASK(g_4e6948, 0x11b8), sizeof(old_11b8)))
	{
		for (dword i = 0; i < 68; i++)
		{
			if (g_440dd8[i].field_20)
				g_440dd8[i].field_20(old_11b8, CLUSTER_MASK(g_4e6948, 0x11b8), bsp->field_9c);
		}
	}
}
