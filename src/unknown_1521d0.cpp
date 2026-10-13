// @flags /O2 /Os /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_timer_player
{
    byte unknown00[0x2c];
    long unit_index;
    byte unknown30[0x198 - 0x30];
    short effect_timers[2];
    byte unknown19c[0x21c - 0x19c];
};

void function_d0e00(long unit_index, real rate);

// @retail 0x1521d0
void function_1521d0(long player_index)
{
	s_timer_player *player = (s_timer_player *)g_4e8c24->data + (player_index & 0xffff);
	long effect;

	for (effect = 0; effect < 2; effect++)
	{
		if (player->effect_timers[effect] > 0)
		{
			player->effect_timers[effect]--;
			if (player->effect_timers[effect] == 0 && effect == 0)
			{
				s_record_pool *players = g_4e8c24;
				function_d0e00(((s_timer_player *)players->data)[player_index & 0xffff].unit_index, 1.0f);
			}
		}
	}
}
