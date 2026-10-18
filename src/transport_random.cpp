#include "unknown_4cf790.h"
// @flags /O2 /Ob1 /Gr
/* TRANSPORT_RANDOM.CPP: random bytes, from XNet once the transport has
   started it (g_4cf790), else from a generator seeded by the clock */

#include "unknown_11c920.h"
#include <xtl.h>
#include <stdlib.h>
#include <time.h>



/* fills a buffer with count random bytes */
// @retail 0x7ad80
void transport_get_random_bytes(long count, byte *buffer)
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
