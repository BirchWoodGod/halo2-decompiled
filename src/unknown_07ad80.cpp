#include "unknown_4cf790.h"
// @flags /O2 /Ob1 /Gr
/* UNKNOWN_07AD80.CPP: network random bytes and address helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <stdlib.h>
#include <time.h>



// @retail 0x7ad80
void function_07ad80(long count, byte *buffer)
{
	if (g_4cf790)
	{
		XNetRandom(buffer, count);
		return;
	}

	dword now = (dword)time(0);
	dword seed = rand() ^ GetTickCount() ^ now;
	for (long i = 0; i < count; i++)
	{
		seed = seed * 0x19660d + 0x3c6ef35f;
		/* the high word scaled to a byte range */
		buffer[i] = (byte)(((seed >> 16) * 256) >> 16);
	}
}
