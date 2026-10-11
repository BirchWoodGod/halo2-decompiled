// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include "unknown_223b60.h"
#include "physical_memory.h"

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
void function_12d520(long a);
double timing_ticks_to_seconds(__int64 ticks);

static __int64 read_tsc(void)
{
	volatile __int64 t = 0;
	__asm rdtsc
}

struct s_key
{
	long a;
	long b;
};

class c_resource_lock
{
public:
	virtual long method0(long handle);
	virtual void method1(long block);

	s_key key;
};

// @retail 0x18f260
long c_resource_lock::method0(long handle)
{
	union s_18f265 { dword field_0; real field_4; };
	static s_18f265 local_6 = { 0x3dcccccd };
	long result;
	long local_0 = this->key.b;
	long local_1 = this->key.a;
	long volatile retries;
	__int64 start = read_tsc();
	result = 0;

	if (handle > 0 && g_4e6464->page_count > 0)
	{
		retries = 0;
		do
		{
			result = function_12d2f0(handle, 0, *(long const volatile *)&local_1, *(long const volatile *)&local_0);
			if (result != 0)
			{
				break;
			}
			long local_2 = retries;
			if (local_2 < 30)
			{
				retries = local_2 + 1;
				function_12c600();
				continue;
			}

			__int64 elapsed = read_tsc() - start;
			if (elapsed < 0)
			{
				elapsed = 0;
			}
			if (!(timing_ticks_to_seconds(elapsed) < local_6.field_4))
			{
				break;
			}
			D3DDevice_KickPushBuffer();
			D3DDevice_IsBusy();
			SwitchToThread();
		} while (result == 0);
	}

	return result;
}

// @retail 0x18f350
void c_resource_lock::method1(long block)
{
	function_12d520(block);
}