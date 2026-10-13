// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0B4050.CPP: counts the retries of a result upload and gives up
   on the server registration after three tries per server (built with SSE,
   apart from its neighbours in unknown_0b3d30.cpp) */
#include "upload_storage.h"

void online_result_registration_clear(void);
void function_6b640(long task_index);
extern bool g_510580;
extern long g_510584;
extern dword g_5107d8;

/* the number of registered servers, none while unregistered */
static __forceinline long registered_count(void)
{
	long count = 0;
	if (g_510580) count = g_5107d8;
	return count;
}

// @retail 0xb4050
void function_b4050(void)
{
	g_547620++;
	if (g_547620 >= 3 * registered_count())
	{
		online_result_registration_clear();
		long task_index = g_510584;
		if (task_index != NONE)
		{
			function_6b640(task_index);
			g_510584 = NONE;
		}
		g_510580 = false;
		g_54761c = 0;
	}
}
