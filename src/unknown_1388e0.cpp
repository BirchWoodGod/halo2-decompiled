// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

void function_19043f(long arg_1, long arg_2, long arg_3);
void function_12b980(void);
void function_1389c0(void);
void function_18e960(void);

static __forceinline bool function_1388e3(long arg_1)
{
	return g_4e6948->state == arg_1;
}

static __forceinline long function_1388e1(long arg_1)
{
	long local_1 = NONE;
	if (arg_1 >= 0 && arg_1 < 3)
		local_1 = arg_1 + 1;
	return local_1;
}

// @retail 0x1388e0
void function_1388e0(void)
{
	s_game_options_view *local_1 = g_4e6948;
	if (local_1 && local_1->flag1120)
	{
		if (local_1->state == 1)
		{
			if (function_1388e3(1) && local_1->position_b != NONE)
			{
				long local_2 = 0;
				do
				{
					function_19043f(local_2, local_1->difficulty, local_1->position_b);
					local_2 = function_1388e1(local_2);
				}
				while (local_2 != NONE);
				function_12b980();
			}
			function_18e960();
		}
		else if (local_1->state == 2)
		{
			function_1389c0();
		}
	}
}
