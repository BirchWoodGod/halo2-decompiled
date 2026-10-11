// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_2521F2.CPP: the screen shown while a multiplayer game is being
   found (vtable 0x45a420, screen 0xe7) */

#include "unknown_11c920.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"

/* the music fades out while the screen shows */
long function_1480ff(long screen_id);
bool function_22f0ff(c_widget *widget);
long function_2197f0(real gain);
void __stdcall function_221980(char const *name, long value_bits, real time);
bool function_68290(void);
void __stdcall function_19a02d(long *string_handle, real *progress);

class c_screen_45a420 : public c_class_1473c9
{
public:
	c_screen_45a420(long a, long b, word user_flags);
	~c_screen_45a420();

	virtual void v3();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();
};

c_class_1473c9 *__stdcall function_2521f8(s_screen_parameters *parameters);

// @retail 0x2521f2
screen_load_proc c_screen_45a420::get_load_proc()
{
	return function_2521f8;
}

// @retail 0x2521f8
c_class_1473c9 *__stdcall function_2521f8(s_screen_parameters *parameters)
{
	c_screen_45a420 *screen;

	parameters->type_bit2 = true;
	screen = new c_screen_45a420(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x252237
c_screen_45a420::c_screen_45a420(long a, long b, word user_flags) :
	c_class_1473c9(0xe7, a, b, user_flags)
{
	if (g_4e6948->state == 2)
	{
		struct { long value; real duration; } fade;
		fade.value = function_2197f0(0.0f);
		fade.duration = 1.0f;
		function_221980("", fade.value, fade.duration);
	}
}

// @retail 0x25229e
c_screen_45a420::~c_screen_45a420()
{
	if (g_4e6948->state == 2)
	{
		struct { long value; real duration; } fade;
		fade.value = function_2197f0(1.0f);
		fade.duration = 1.0f;
		function_221980("", fade.value, fade.duration);
	}
}

// @retail 0x252280 deleting c_screen_45a420

/* builds the screen from its definition (its slot 18 is shared by the screens
   of 0x4549c0, 0x45a250, 0x45ac80 and 0x45bcd0) */
// @retail 0x2522d7
void c_screen_45a420::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
}

/* shows the search's state and progress, and leaves once it is over */
// @retail 0x252325
void c_screen_45a420::v3()
{
	if (!function_22f0ff((c_widget *)this))
	{
		if (function_68290())
		{
			c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 1, false);
			c_class_2b01eb *bar = (c_class_2b01eb *)find_child(8, 0, false);
			long string_handle = 0;
			real progress = 0.0f;

			function_19a02d(&string_handle, &progress);
			if (text && string_handle)
			{
				text->function_253b1a(string_handle);
			}
			if (bar)
			{
				bar->value84 = progress;
			}
		}
		else
		{
			start_animation(3);
		}
	}
	c_class_1a2c81::v3();
}

/* the online task screen of leaving the squad: shows its description, and
   finishes at once when there was nothing to leave (the screen is declared
   in full by unknown_1a2ca7.cpp; only the method called here) */
class c_online_task_screen
{
public:
	void set_description(long string_handle);
};

void online_task_screen_finish(c_online_task_screen *screen);
long function_19a279(void);

// @retail 0x2523bc
void __stdcall function_2523bc(c_online_task_screen *screen)
{
	long result = function_19a279();

	screen->set_description(0xd000443);
	if (!result)
	{
		online_task_screen_finish(screen);
	}
}
