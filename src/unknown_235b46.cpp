// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_235B46.CPP: the main menu's music: the screens that want it, the
   looping sound that plays it, and how long it has been silent */

#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"

struct s_type_954545;
s_type_954545 *user_interface_globals_get(void);
long function_147f4f();
long function_18a5a0(long tag_index, long object_index, real value);
word *function_18a5e0(long datum_index);
long map_location_progress_get(char const *map_name);
long function_122db0(long mode, char const *map_name);

/* the user interface globals' music */
struct s_user_interface_globals_music
{
	byte unknown00[0x1bc];
	/* the looping sound of the main menu's music */
	long music_tag_index;
	/* how long the music stays silent (milliseconds) */
	long silence_time;
};

/* the main menu's (scenario's) map type */
struct s_scenario_type_view
{
	byte unknown00[0x10];
	short type;
};

/* the last entry of the campaign (unknown_147f6d.cpp): its map name */
struct s_campaign_entry_view
{
	byte unknown00[8];
	char map_name[1];
};
struct s_entry_a;
s_entry_a *function_148d61();

/* the music's state */
struct s_main_menu_music
{
	/* whether it plays now, and whether it should */
	long playing;
	long wanted;
	/* whether the current screen wants it */
	long enabled;
	/* when it last started or stopped (user interface time) */
	dword time;
	/* its looping sound */
	long sound_index;
	/* when it stopped (tick count) */
	dword stop_tick;
};

extern bool g_4701b8;

/* the user interface time (the window manager's) */
extern dword g_54d5b8;

// @retail 0x235b46
long main_menu_music_tag_index()
{
	long result = NONE;
	s_user_interface_globals_music *globals = (s_user_interface_globals_music *)user_interface_globals_get();

	if (globals)
	{
		result = globals->music_tag_index;
	}
	return result;
}

/* (unknown_223976.cpp calls it function_235ca9) */
// @retail 0x235ca9
dword function_235ca9()
{
	dword result = 0;
	s_user_interface_globals_music *globals = (s_user_interface_globals_music *)user_interface_globals_get();

	if (globals)
	{
		result = globals->silence_time;
	}
	return result;
}

// @retail 0x235b5d
void main_menu_music_start(s_main_menu_music *music)
{
	long tag_index = main_menu_music_tag_index();

	if (tag_index != NONE)
	{
		music->sound_index = function_18a5a0(tag_index, NONE, 1.0f);
	}
}

// @retail 0x235b82
void main_menu_music_stop(s_main_menu_music *music)
{
	if (music->sound_index != NONE)
	{
		function_18a5e0(music->sound_index);
		music->sound_index = NONE;
		music->stop_tick = GetTickCount();
	}
}

/* starts or stops the music as wanted, at most every six seconds */
// @retail 0x235b9c
void main_menu_music_update_playing(s_main_menu_music *music)
{
	if (music->playing != music->wanted)
	{
		dword time = g_54d5b8;

		if (!music->time || (long)(g_54d5b8 - music->time) > 6000)
		{
			if (music->wanted == 1)
			{
				main_menu_music_start(music);
			}
			else
			{
				main_menu_music_stop(music);
			}
			music->playing = music->wanted;
			music->time = time;
		}
	}
}

/* the campaign screen wants the music until the campaign is far enough in */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x235c4a
void main_menu_music_check_campaign(s_main_menu_music *music)
{
	s_campaign_entry_view *entry = (s_campaign_entry_view *)function_148d61();
	char const *map_name = 0;
	long progress = 3;

	if (entry)
	{
		map_name = entry->map_name;
		if (map_name)
		{
			progress = map_location_progress_get(map_name);
		}
	}
	switch (progress)
	{
	case 0:
	case 3:
		music->enabled = 1;
		break;
	case 1:
	{
		long volatile limit = function_235ca9();
		long current = function_122db0(0, map_name);
		long threshold = limit;
		_ReadWriteBarrier();
		if (current <= threshold)
		{
			music->enabled = 0;
		}
		break;
	}
	case 2:
		music->enabled = 0;
		break;
	}
}

/* whether the current screen wants the music */
// @retail 0x235bfa
void main_menu_music_check_screen(s_main_menu_music *music)
{
	s_scenario_type_view *scenario = (s_scenario_type_view *)g_4e0350;

	if (scenario && scenario->type == 2)
	{
		switch (function_147f4f())
		{
		case 6:
		case 9:
			music->enabled = 1;
			return;
		case 13:
		case 14:
		case 0xba:
			break;
		case 0xd1:
			main_menu_music_check_campaign(music);
			return;
		default:
			return;
		}
	}
	music->enabled = 0;
}

// @retail 0x235bdb
void main_menu_music_update(s_main_menu_music *music)
{
	main_menu_music_check_screen(music);
	long wanted = music->enabled;

	if (!g_4701b8)
	{
		wanted = 0;
	}
	music->wanted = wanted;
	main_menu_music_update_playing(music);
}

/* whether the music has been silent for its silence time */
// @retail 0x235cbf
bool main_menu_music_silence_done(s_main_menu_music *music)
{
	if (!music->enabled && music->sound_index == NONE)
	{
		dword silence_time = function_235ca9();

		return silence_time < GetTickCount() - music->stop_tick;
	}
	return false;
}

/* how far into its silence time the music is (0 to 1) */
// @retail 0x235ce6
real main_menu_music_silence_fraction(s_main_menu_music const *music)
{
	real result = 0.0f;

	if (!music->enabled && music->sound_index == NONE)
	{
		real silence_time = (real)(long)function_235ca9();
		real elapsed = (real)(GetTickCount() - music->stop_tick);

		if (silence_time > 1.0f)
		{
			result = elapsed / silence_time;
			if (result < 0.0f)
			{
				result = 0.0f;
			}
			else if (result > 1.0f)
			{
				result = 1.0f;
			}
		}
	}
	return result;
}
