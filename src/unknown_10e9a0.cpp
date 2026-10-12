// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_10E9A0.CPP */

#include "unknown_11c920.h"
#include "unknown_10aca0.h"

void function_10e9f0(long object_index, short channel, real value, real time);

/* the names of the channels */
long const g_4408bc[13] = { 0x06000084, 0x070001a4, 0x050001a5, 0x0b0001a6, 0x080001a7, 0x0c0001a8, 0x070001a9, 0x060001aa, 0x080001ab, 0x070001ac, 0x050001ad, 0x070001ae, 0x040001af };

// @retail 0x10e9a0
void function_10e9a0(long object_index, long name, real value, real time)
{
	if (name == 0x7000001)
	{
		function_10e9f0(object_index, NONE, value, time);
		return;
	}
	for (long index = 0; index < 13; index++)
	{
		if (name == g_4408bc[index])
		{
			function_10e9f0(object_index, (short)index, value, time);
			return;
		}
	}
	function_10e9f0(object_index, NONE, value, time);
}
