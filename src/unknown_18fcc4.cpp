// @flags /O1 /Oi /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include <string.h>

struct s_player_profile;

struct s_18fcc4
{
	dword field_0;
	byte field_4[0x18 - 4];
	s_player_profile_settings field_18;
	byte field_1f8[0xc70 - 0x1f8];
};

bool game_loop_running(void);
void __stdcall function_18fb34(long arg_0, s_player_profile_settings *arg_1, long arg_2);
void __stdcall function_2153dd(long arg_0, long arg_1, s_player_profile_settings *arg_2, long arg_3);
void __stdcall function_18fd20(long arg_0, s_player_profile_settings *arg_1, long arg_2);

// @retail 0x18fcc4
void __stdcall function_18fcc4(long arg_0, s_player_profile *arg_1, long arg_2)
{
	if (game_loop_running() && g_4e6948->state != 3)
	{
		s_18fcc4 *local_0 = (s_18fcc4 *)g_54e8e0 + arg_0;
		if ((local_0->field_0 & 0x10) && memcmp(&local_0->field_18, arg_1, 0x1e0))
		{
			function_18fb34(arg_0, (s_player_profile_settings *)arg_1, arg_2);
			local_0->field_0 |= 0x40;
		}
	}
	else
		function_18fd20(arg_0, (s_player_profile_settings *)arg_1, arg_2);
}

// @retail 0x18fd20
void __stdcall function_18fd20(long arg_0, s_player_profile_settings *arg_1, long arg_2)
{
	s_18fcc4 *local_0 = (s_18fcc4 *)g_54e8e0 + arg_0;
	if (local_0->field_0 & 0x10)
	{
		if ((local_0->field_0 & 0x40) || memcmp(&local_0->field_18, arg_1, 0x1e0))
		{
			function_18fb34(arg_0, arg_1, arg_2);
			if (arg_2 != NONE)
				function_2153dd(arg_0, arg_2, arg_1, 0);
			local_0->field_0 &= ~0x40;
		}
	}
}
