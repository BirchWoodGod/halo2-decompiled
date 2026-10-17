// @flags /O1 /Ob1 /arch:SSE /Gr
/* UNKNOWN_223976.CPP: the attract, intro and credits movies (retail's
   unknown_223976.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "files.h"

s_type_acf665 *file_ref_create(s_type_acf665 *file, bool replace, const char *name);
bool function_1368f0(s_type_acf665 *file);
char *csprintf_256(char *buffer, const char *format, ...);
void function_156090(char const *name, dword flags);
bool function_155f60(void);

extern dword g_51ebec;
extern dword g_54d5b8;
dword g_54d5bc;

bool function_138800();
bool function_163870(void);
bool network_session_manager_session_unready(void);
long function_147f4f();
dword function_235ca9(void);

/* whether the main menu may count down to the attract movie, and how long
   it waits (in milliseconds) */
bool g_4701b8 = true;
dword g_4701bc = 75000;

/* whether the attract movie should start: the main menu has been idle for
   long enough */
// @retail 0x223976
bool function_223976(void)
{
	volatile bool result = false;

	if (function_138800() && g_4e6948->state == 3 && function_163870() && !network_session_manager_session_unready() &&
		!function_155f60())
	{
		dword idle_time = g_54d5b8 - (g_51ebec > g_54d5bc ? g_51ebec : g_54d5bc);
		bool local_b608f1 = function_147f4f() == 9 || function_147f4f() == 6;
		dword delay = function_235ca9();

		g_4701b8 = !(idle_time >= g_4701bc - delay && local_b608f1);
		if (idle_time >= g_4701bc && local_b608f1)
		{
			result = true;
		}
	}
	return result;
}

/* the path of a movie, which is looked up on the disc */
// @retail 0x223aa8
bool attract_mode_movie_path(char *path, char const *name)
{
	s_type_acf665 file;

	csprintf_256(path, "d:\\bink\\%s_%d.bik", name, 60);
	function_1368f0(file_ref_create(&file, false, path));
	return true;
}

/* the path of the attract (0), intro (1) or credits movie */
// @retail 0x223a7d
bool attract_mode_movie_path_by_type(long type, char *path)
{
	switch (type)
	{
	case 0:
		return attract_mode_movie_path(path, "attract");
	case 1:
		return attract_mode_movie_path(path, "intro");
	default:
		return attract_mode_movie_path(path, "credits");
	}
}

/* plays the attract movie */
// @retail 0x223a21
void function_223a21(void)
{
	char path[256];
	long type = random_index(&g_4e7408->seed, 1);

	path[0] = 0;
	if (attract_mode_movie_path_by_type(type, path))
	{
		function_156090(path, 14);
	}
	if (!function_155f60())
	{
		g_51ebec = g_54d5b8;
	}
}
