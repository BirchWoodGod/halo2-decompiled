#include "unknown_24fa12.h"
#include "unknown_11c920.h"
#include <stdlib.h>
#include <string.h>

// @flags /O1 /Gr

// @retail 0x24fa12
c_session_user_state::c_session_user_state()
{
	index = NONE;
	active = false;
}

struct s_session_screen_state
{
	byte unknown00[0x814];
	c_session_user_state users[4];
};

// @retail 0x24fa23
long function_24fa23(s_session_screen_state const *screen)
{
	return screen->users[0].active || screen->users[1].active ||
		screen->users[2].active || screen->users[3].active;
}

c_class_1473c9 *__stdcall function_24fa4c(s_screen_parameters *parameters);

// @retail 0x24fa1d
screen_load_proc c_screen_24fd74::get_load_proc()
{
	return function_24fa4c;
}

// @retail 0x24fa4c
c_class_1473c9 *__stdcall function_24fa4c(s_screen_parameters *parameters)
{
	c_screen_24fd74 *screen = new c_screen_24fd74(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x24fa88
c_screen_24fd74::c_screen_24fd74(long a, long b, word user_flags) :
	c_class_1473c9(0xe, a, b, user_flags),
	button0(0, user_flags),
	button1(1, user_flags),
	value810(true),
	value811(true),
	value812(0),
	value813(false),
	countdown_stage(NONE),
	countdown_time(0),
	mode(0),
	value142e(false),
	choice0(this, (session_choice_method)&c_screen_24fd74::handle_lobby_choice),
	choice1(this, (session_choice_method)&c_screen_24fd74::handle_lobby_choice),
	value1460(false),
	value1464(0),
	value1468(false),
	value146c(NONE),
	state_shown_time(0)
{
}

// @retail 0x24fba2 destructor c_screen_24fd74

// @retail 0x24fb86 deleting c_screen_24fd74

typedef char session_screen_size_check[sizeof(c_screen_24fd74) == 0x1474 ? 1 : -1];

extern bool g_51ec98;
bool function_199971(void);
bool function_592f0(void);
long window_manager_get_game_variant_index(void);
bool function_199e6d(long index);
void function_19a0af(long value);
long player_slot_find_signed_in(long mode);
bool function_19a78e(long controller, long countdown, long minimum);
void function_236299(long sound);
bool function_2510e1(void);

// @retail 0x24fbea
void function_24fbea(c_screen_24fd74 *screen)
{
	if (function_199971())
	{
		if (function_592f0() && g_51ec98 && function_199e6d(window_manager_get_game_variant_index()))
		{
			function_19a0af(window_manager_get_game_variant_index());
			if (!function_19a78e(player_slot_find_signed_in(0), 3, 3))
				function_236299(2);
		}
	}
	else if (!function_2510e1())
		screen->value142e = true;
}

long function_19989d(void);

// @retail 0x24ff0a
void function_24ff0a(c_screen_24fd74 *screen)
{
	short mode = 0;
	switch (function_19989d())
	{
	case 0: mode = 1; break;
	case 1: mode = 0; break;
	case 2: mode = 1; break;
	case 3: mode = 0; break;
	case 4: mode = 1; break;
	case 5: mode = 0; break;
	case 6: mode = 2; break;
	default: mode = screen->mode; break;
	}
	if (mode != screen->mode)
	{
		long pane = mode;
		screen->function_230427((short *)&pane);
		screen->mode = mode;
	}
}

struct s_session_player_view;
bool function_19a902(void);
long function_251364(s_session_player_view *player);

// @retail 0x24ff61
void function_24ff61(c_class_1a2c81 *screen, bool selected, s_session_player_view *player)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)screen->find_child(6, 1, false);
	if (text)
	{
		long string_handle;
		if (function_19a902())
			string_handle = selected ? 0x2700021b : 0x2000021a;
		else
		{
			if (selected)
				string_handle = (byte)function_251364(player) ? 0x25000217 : 0x26000216;
			else
				string_handle = (byte)function_251364(player) ? 0x1d000219 : 0x1e000218;
		}
		text->function_253b1a(string_handle);
	}
}

long function_1480ff(long screen_id);
bool function_13ee20(word const *string, long font);
void function_199b45(void);
byte *function_19aaa5(long player_index);

// @retail 0x24fc53
void c_screen_24fd74::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	c_class_1a2c81 *buttons[2] = { &button0, &button1 };
	s_screen_layout layout = { 0, 1, { { 2, buttons, 0, 0 } } };
	build(&layout);
	delegate_register(&button0.handlers, &choice0);
	delegate_register(&button1.handlers, &choice1);
	value1464 = 0;
	value146c = NONE;
	value1468 = false;
	value1460 = false;
	c_class_1a2c81::v1();
	v7(&button0);
	if (!(*(byte *)parameters & 1))
		function_24fbea(this);
	function_13ee20((word const *)L"0123456789", 4);
	g_51ec98 = false;
}

// @retail 0x24fd21
void c_screen_24fd74::v2()
{
	if (value1468)
	{
		function_199b45();
		value1468 = false;
	}
	c_class_1a2c81::v2();
}

// @retail 0x24fd41
int __cdecl function_24fd41(void const *left, void const *right)
{
	/* The comparator's first argument is stack-resident in retail. */
	void const *const *local_left_reference = &left;
	byte *local_left = function_19aaa5(*(long const *)*local_left_reference);
	byte *local_right = function_19aaa5(*(long const *)right);
	char a = *(char *)(local_left + 0x7c);
	char b = *(char *)(local_right + 0x7c);
	return a > b ? 1 : a < b ? -1 : 0;
}

long function_19a2ce(real *arg_0);
long function_19a50f(long arg_0);
long network_session_interface_find_player_49b8();
bool function_19abe4(long arg_0);
bool function_19a8ef();
bool function_199994();
byte function_199eaa();
long function_251139();
long function_63ec0();
long function_199ebc();
bool function_251188(long arg_0);

// @retail 0x24ffc9
void function_24ffc9(c_screen_24fd74 *arg_0, bool arg_1)
{
	c_screen_24fd74 *const *local_0 = &arg_0;
	short local_1;
	switch ((*local_0)->mode)
	{
	case 0: local_1 = 2; break;
	case 1: local_1 = 2; break;
	case 2: local_1 = 2; break;
	default: local_1 = NONE; break;
	}
	c_text_widget_45a5e0 *local_2 = (c_text_widget_45a5e0 *)(*local_0)->find_text(local_1);
	if (local_2)
	{
		long local_3 = function_19a2ce(0);
		long local_4;
		switch (local_3)
		{
		case 10:
		{
			long local_5 = network_session_interface_find_player_49b8();
			if (local_5 != NONE)
				local_4 = function_19abe4(local_5) ? 0x1200020d : 0x1b00020c;
			else if (function_19a8ef())
				local_4 = function_199994() ? 0x1500015d : 0xa000208;
			else
				local_4 = arg_1 ? 0x200001f0 : 0x2b00020b;
			break;
		}
		case 8:
			local_4 = function_199eaa() ? 0x170001ee : 0x100001ed;
			break;
		case 7:
			if (function_199994())
				local_4 = 0xe0001ec;
			else
			{
				switch (function_251139())
				{
				case 2: local_4 = 0xc0001e5; break;
				case 3: local_4 = 0xc0001e6; break;
				case 4: local_4 = 0xc0001e7; break;
				case 5: local_4 = 0xc0001e8; break;
				case 6: local_4 = 0xc0001e9; break;
				case 7: local_4 = 0xc0001ea; break;
				case 8: local_4 = 0xc0001eb; break;
				default: local_4 = 0xc0001e5; break;
				}
			}
			break;
		default:
			local_4 = function_19a50f(local_3);
			break;
		}
		if (local_3 == 9)
		{
			long local_5 = function_63ec0();
			if (function_199ebc() > local_5)
				local_4 = (*local_0)->get_definition_value(0, 3);
		}
		local_2->function_253b1a(local_4);
		c_class_22cc8e *local_5 = local_2->function_22f52e();
		if (function_251188(local_3))
			((byte *)local_5)[0x14] |= 1;
		else
			((byte *)local_5)[0x14] &= ~1;
	}
}


byte *network_session_interface_get_data_4db0();
word function_1901fc();
long player_slot_iterate_next(long arg_0);
bool function_19a951(long arg_0);
long function_19aa17(long arg_0);
bool function_251121();
void function_250dae(c_screen_24fd74 *arg_0);
void function_250332(c_class_1a2c81 *arg_0);
void function_2503c0(c_class_1a2c81 *arg_0);
struct s_game_variant;
struct s_session_flags_view;
void function_2501c0(c_screen_24fd74 *arg_0, s_game_variant *arg_1);
void function_25042d(c_screen_24fd74 *arg_0, long arg_1, s_session_flags_view *arg_2,
    long *arg_3, long *arg_4, long arg_5);
void function_2507dc();

#pragma intrinsic(memset)
#pragma inline_depth(0)
// @retail 0x24fd74
void c_screen_24fd74::v3()
{
    long local_0 = function_19989d();
    byte *local_1 = network_session_interface_get_data_4db0();
    bool local_2 = function_592f0();
    bool local_3 = function_199994();
    if (local_1 && !*(long *)(local_1 + 0x44))
        local_1 = 0;
    if (value142e && function_251121())
        value142e = false;
    set_user_flags(function_1901fc());
    *(short *)&user_flags = NONE;
    if (!(byte)function_251364((s_session_player_view *)local_1))
    {
        for (long local_4 = 0; local_4 != NONE; local_4 = player_slot_iterate_next(local_4))
        {
            teams[local_4].valid = false;
            teams[local_4].team = NONE;
        }
    }
    long local_5[4];
    for (long local_6 = 0; local_6 != NONE; local_6 = player_slot_iterate_next(local_6))
        local_5[local_6] = function_19aa17(local_6);
    long local_7 = 0;
    long local_8[16];
    memset(local_8, 0xff, sizeof(local_8));
    for (long local_9 = 0; local_9 < 16; ++local_9)
    {
        if (function_19a951(local_9))
            local_8[local_7++] = local_9;
    }
    if (!local_3 && local_1 && (local_1[0x48] & 1))
        qsort(local_8, local_7, sizeof(long), function_24fd41);
    function_250eb7();
    update_countdown(local_2);
    if (local_0 != NONE)
    {
        function_24ff0a(this);
        function_250dae(this);
        function_24ff61(this, local_2, (s_session_player_view *)local_1);
        function_24ffc9(this, local_2);
        function_250155();
        if (local_3)
            function_2503c0(this);
        else
            function_2501c0(this, (s_game_variant *)local_1);
        function_250332(this);
        function_2508a8();
        function_25042d(this, local_0, (s_session_flags_view *)local_1, local_8, local_5, local_7);
        function_2507dc();
        show_session_state();
    }
    function_250f3a(local_1);
    c_class_1a2c81::v3();
}
#pragma inline_depth(255)
