// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_241620.CPP: a player touched a unit of another team (or, in
   mode 9, of its own): an engine callback that scores it and reports it */

#include "unknown_11c920.h"
#include "globals.h"
#include "game_engine_events.h"
#include "unknown_2420a0.h"

extern s_slot_table *g_51ec80;

void function_1967d0(long a, long c, long b, long delta);
void function_1970a0(long b, long a, long delta);
void function_242660(long object_index, bool arg_1);
void function_2430a0(long player_index, bool flag);

/* the team an event reports: none for team 8 */
static __forceinline short event_team(short team)
{
	return team == 8 ? (short)NONE : team;
}

// @retail 0x241620 standard
void __stdcall function_241620(long player_index, long unit_index)
{
	s_event_player *player = event_player_get(player_index);
	long absolute_index = player_index & 0xffff;
	byte *unit = *(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8);
	long team = *(short *)(unit + 0x17e);
	if (team == NONE || team == 8)
		return;
	s_game_options_view *options = g_4e6948;
	if ((options->mode_180 != 9 && team == player->team) || (options->mode_180 == 9 && team != player->team))
	{
		if (options->flags22c & 4)
		{
			if (options->mode != 4 && !options->flag1128 && (g_51ec80->flags[team] & 1))
			{
				if (options->mode_180 == 9)
					function_1967d0(absolute_index, player->team, 0x16, 1);
				else
					function_1967d0(absolute_index, player->team, 0x11, 1);
				function_1970a0(absolute_index, 0x14, 1);
				s_event event;
				game_engine_event_initialize(&event, options->mode_180 == 9 ? 10 : 3, 3);
				game_engine_event_set_cause_player(&event, player_index);
				event.effect_team = event_team((short)team);
				function_19eb90(&event);
				function_242660(unit_index, false);
			}
		}
		else if (g_51ec80->flags[team] & 1)
			function_2430a0(player_index, false);
	}
}
