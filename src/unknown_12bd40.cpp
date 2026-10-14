// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "main_globals.h"
#include "main_messages.h"

bool __stdcall function_11c1b0(short arg_0, bool arg_1);
void function_1a0180(long arg_0, long arg_1, word *arg_2);
void function_18efa0(void);

// @retail 0x12bd40
void function_12bd40(void)
{
	bool local_0 = function_11c1b0(main_globals.field_0_3, true);
	if (g_4e6948->state == 1)
	{
		function_24cdaf();
		long local_1 = NONE;
		for (long local_2 = 0; local_2 < 4; local_2++)
		{
			if (g_4e8c20->entries[local_2] != NONE)
			{
				local_1 = local_2;
				break;
			}
		}
		word local_3[256];
		local_3[0] = 0;
		if (g_510c94 && g_510c94->string_list != NONE)
			function_1a0180(g_510c94->string_list, 0xf0006a3, local_3);
		function_24cbee(local_1, local_3);
	}
	if (!local_0)
		function_18efa0();
	main_globals.switch_structure_bsp = false;
	main_globals.field_0_3 = NONE;
}

#include "screen_widgets.h"
#include "unknown_058ee0.h"
#include "unknown_2b116a.h"
class c_slots_215541;
struct s_storage_request;
extern dword g_54d5b8;
extern bool g_4ee4e0;
s_storage_request *__stdcall function_215454(c_slots_215541 *arg_0, byte arg_1);
void __stdcall function_215641(c_slots_215541 *arg_0);
long __stdcall function_2155f4(c_slots_215541 *arg_0, long *arg_1, real *arg_2, long *arg_3);
void function_1241b0(s_saved_game_read *arg_0);
void function_190e71(long arg_0);
word function_1901fc(void);
c_class_1473c9 *__stdcall progress_screen_load(s_screen_parameters *arg_0);
void __stdcall function_18f1c0(long arg_0);

struct s_12bbd0
{
	dword field_0;
	long field_4[8];
};

struct s_loop_allocator;
extern s_loop_allocator *g_51e998;
bool loop_allocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line);
void function_147ebe(void);

#pragma inline_depth(0)
// @retail 0x12bbd0
void function_12bbd0(void)
{
	void *pointer;
	if (!loop_allocate(g_51e998, &pointer, sizeof(s_12bbd0), 0, 0))
	{
		function_147ebe();
		if (!loop_allocate(g_51e998, &pointer, sizeof(s_12bbd0), 0, 0))
			pointer = 0;
	}
	s_12bbd0 *local_0 = (s_12bbd0 *)pointer;
	if (local_0)
	{
		local_0->field_0 = g_54d5b8;
		for (long local_1 = 0; local_1 < 8; local_1++)
			local_0->field_4[local_1] = 0;
	}
	if (g_4e6948->state == 1 && !*((byte *)g_4e6948 + 0x134))
		function_1241b0((s_saved_game_read *)function_215454((c_slots_215541 *)local_0, false));
	function_190e71((long)local_0);
	if (!g_510c54->active || !g_510c54->unknown01)
		g_510c54->unknown01 = true;
	s_screen_parameters local_2;
	local_2.field_c = 0;
	local_2.type = 0;
	local_2.user_flags = function_1901fc();
	local_2.a = 1;
	local_2.b = 4;
	local_2.id[0] = NONE;
	local_2.id[1] = NONE;
	local_2.id[2] = NONE;
	local_2.load = progress_screen_load;
	byte *local_3 = (byte *)progress_screen_load(&local_2);
	*(s_12bbd0 **)(local_3 + 0x610) = local_0;
	*(long *)(local_3 + 0x624) = 0;
	*(long *)(local_3 + 0x61c) = (long)function_2155f4;
	*(long *)(local_3 + 0x620) = (long)function_215641;
	g_4ee4e0 = true;
	function_18f1c0(g_4e6948->state == 1 && *((byte *)g_4e6948 + 0x134) ? 3 : 0);
	main_globals.quit_game = false;
}
#pragma inline_depth(255)
