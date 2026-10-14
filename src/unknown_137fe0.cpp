// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_137FE0.CPP: one game tick: reads the next input block, then
   updates every subsystem in turn and advances the game time */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_067e10.h"
#include <float.h>

struct s_controller_sticks_view;

/* the timer at g_510c6c (unknown_1552e0.cpp), as read here */
struct s_137fe0_timer
{
	byte unknown00[4];
	real elapsed;
	byte unknown08[0x14 - 0x8];
	long ticks_left;
};

struct s_unknown_78;
extern s_unknown_78 *g_510c6c;

void function_682e0(s_simulation_block_data *block);
void __stdcall function_683a0(byte *block);
void function_684e0(byte *block);
void __stdcall function_14d3a0(byte const *input);
void function_138db0(s_controller_sticks_view const *input);
void function_1c83c0();
void function_1fb510(void);
void function_20b660(void);
void function_15c150();
void function_195ed0(void);
void function_209310(void);
void function_138c10(void);
void function_b6f10();
void function_1c3d30();
void function_b7060();
void function_b7150();
void impacts_update(void);
void function_176e40(void);
void function_c1400();
void function_15ca10();
void function_14d8b0(void);
void function_1df500(void);
void function_138e40();
void function_138eb0();
void function_226050(void);
void function_13c680(void);
void function_123430(void);
void function_225730(void);
void function_165dc1(void);
void function_153950(void);
void function_1917b2(void);
void function_16f200(void);

static __forceinline void tick_update(s_simulation_block_data *block)
{
	function_14d3a0((byte const *)block);
	function_138db0((s_controller_sticks_view const *)block);
	function_1c83c0();
	function_1fb510();
	function_20b660();
	function_15c150();
	function_195ed0();
	function_209310();
	function_138c10();
	function_b6f10();
	function_1c3d30();
	function_b7060();
	function_b7150();
	impacts_update();
	function_176e40();
	function_c1400();
	function_15ca10();
	function_14d8b0();
	function_1df500();
	function_138e40();
	function_138eb0();
	function_226050();
	function_13c680();
	function_123430();
	function_225730();
	function_165dc1();
	function_153950();
	function_1917b2();
	function_16f200();
	s_137fe0_timer *timer = (s_137fe0_timer *)g_510c6c;
	timer->ticks_left = timer->ticks_left - 1 < 0 ? 0 : timer->ticks_left - 1;
	timer->elapsed += g_510c54->rate;
}

static __forceinline bool tick_finish(s_simulation_block_data *block)
{
	function_684e0((byte *)block);
	return block->bytes[0] != 0;
}

/* "standard" stops LTCG assuming an 8-aligned entry: it gives retail's frame
   (0x4048) and late ebx/esi saves, and keeps 0x69350, 0x137f70 and 0xb7150
   matching */
// @retail 0x137fe0 standard
void function_137fe0(void)
{
	s_simulation_block_data block;
	_control87(0x9001f, 0xfffff);
	function_682e0(&block);
	function_683a0((byte *)&block);
	if (block.bytes[0])
		tick_update(&block);
	if (tick_finish(&block))
		g_510c54->game_time++;
}
