// @flags /O2 /Gr
/* UNKNOWN_0B2440.CPP: the view establishment (view-establishment,
   player-acknowledge), synchronous (synchronous-update, -actions, -join,
   -gamestate), game results and test message codecs, and the functions
   that register them. A decoder returns whether the message it read is
   valid; the synchronous update and actions messages also have a comparison */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "network_message_types.h"

struct s_view_establishment_message
{
	long mode;
	long value;
};

struct s_player_acknowledge_message
{
	dword mask;
	dword acknowledged;
	byte entries[16][12];
};

struct s_synchronous_actions_message
{
	long unknown00;
	long unknown04;
	bool flag;
	byte unknown09[3];
	dword mask;
	s_player_action actions[4];
};

struct s_synchronous_join_message
{
	long unknown00;
};

struct s_synchronous_gamestate_message
{
	long unknown00;
	long size;
	byte data[1];
};

struct s_synchronous_update_message
{
	byte unknown00[0x4048];
};

struct s_game_results_message
{
	dword unknown00;
	dword unknown04;
	byte results[0x4fb0];
};

struct s_test_message
{
	bool flag;
	byte unknown01[3];
	long value;
};

// @retail 0xb2440
void __stdcall function_b2440(s_bitstream *stream, long size, s_view_establishment_message *message)
{
	stream_write_checked(stream, message->mode, 3);
	stream_write_bit(stream, message->value != NONE);
	if (message->value != NONE)
		function_195720(stream, message->value, 32);
}

// @retail 0xb24e0
bool __stdcall function_b24e0(s_bitstream *stream, long size, s_view_establishment_message *message)
{
	message->mode = function_1959c0(stream, 3);
	if (function_1957d0(stream))
		message->value = function_1959c0(stream, 32);
	else
		message->value = NONE;
	if (!stream_overflowed(stream) && message->mode >= 0 && message->mode < 6 && (message->value == NONE || message->value >= 0))
		return true;
	return false;
}

// @retail 0xb2550
void __stdcall function_b2550(s_bitstream *stream, long size, s_player_acknowledge_message *message)
{
	stream_write_checked(stream, message->mask, 16);
	stream_write_checked(stream, message->acknowledged, 16);
	for (long i = 0; i < 16; i++)
	{
		if (message->mask & (1 << i))
			function_1955d0(stream, message->entries[i], 0x60);
	}
}

// @retail 0xb2600
bool __stdcall function_b2600(s_bitstream *stream, long size, s_player_acknowledge_message *message)
{
	message->mask = function_1959c0(stream, 16);
	message->acknowledged = function_1959c0(stream, 16);
	for (long i = 0; i < 16; i++)
	{
		if (message->mask & (1 << i))
			function_195820(stream, message->entries[i], 0x60);
	}
	if (!stream_overflowed(stream) && (message->acknowledged & ~message->mask) == 0)
		return true;
	return false;
}

// @retail 0xb2680
void network_message_types_register_view_establishment(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_view_establishment, "view-establishment", 8, function_b2440, function_b24e0);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_acknowledge, "player-acknowledge", 0xc8, function_b2550, function_b2600);
}

// @retail 0xb2710
void __stdcall function_b2710(s_bitstream *stream, long size, s_synchronous_update_message *message)
{
	function_87d00(stream, message);
}

// @retail 0xb2730
bool __stdcall function_b2730(s_bitstream *stream, long size, s_synchronous_update_message *message)
{
	bool result = function_87e90(stream, message);
	if (!stream_overflowed(stream) && result)
		return true;
	return false;
}

// @retail 0xb2770
bool __stdcall function_b2770(long size, s_synchronous_update_message *a, s_synchronous_update_message *b)
{
	return function_88060(a, b);
}

// @retail 0xb2790
void __stdcall function_b2790(s_bitstream *stream, long size, s_synchronous_actions_message *message)
{
	function_195720(stream, message->unknown00, 32);
	function_195720(stream, message->unknown04 + 1, 32);
	stream_write_bit(stream, message->flag);
	stream_write_checked(stream, message->mask, 4);
	for (long i = 0; i < 4; i++)
	{
		if (message->mask & (1 << i))
			function_86f90(stream, &message->actions[i]);
	}
}

// @retail 0xb2860
bool __stdcall function_b2860(s_bitstream *stream, long size, s_synchronous_actions_message *message)
{
	bool result = true;
	message->unknown00 = function_1959c0(stream, 32);
	message->unknown04 = function_1959c0(stream, 32) - 1;
	message->flag = function_1957d0(stream);
	message->mask = function_1959c0(stream, 4);
	for (long i = 0; i < 4; i++)
	{
		if (message->mask & (1 << i))
			result = result && function_874c0(stream, &message->actions[i]);
	}
	if (result && !stream_overflowed(stream) && message->unknown00 >= 0 && message->unknown04 >= NONE)
		return true;
	return false;
}

// @retail 0xb2920
bool __stdcall function_b2920(long size, s_synchronous_actions_message *a, s_synchronous_actions_message *b)
{
	bool result = true;
	if (a->mask == b->mask)
	{
		for (long i = 0; i < 4; i++)
		{
			if (a->mask & (1 << i))
				result = result && function_87830(&a->actions[i], &b->actions[i]);
		}
	}
	else
		result = false;
	return result;
}

// @retail 0xb2980
void __stdcall function_b2980(s_bitstream *stream, long size, s_synchronous_join_message *message)
{
	function_195720(stream, message->unknown00, 32);
}

// @retail 0xb29a0
bool __stdcall function_b29a0(s_bitstream *stream, long size, s_synchronous_join_message *message)
{
	message->unknown00 = function_1959c0(stream, 32);
	if (!stream_overflowed(stream) && message->unknown00 >= 0)
		return true;
	return false;
}

// @retail 0xb29e0
void __stdcall function_b29e0(s_bitstream *stream, long size, s_synchronous_gamestate_message *message)
{
	byte *data = message->data;
	stream_write_checked(stream, message->unknown00, 23);
	stream_write_checked(stream, message->size, 13);
	if (message->size > 0)
		function_1955d0(stream, data, message->size * 8);
}

// @retail 0xb2a90
bool __stdcall function_b2a90(s_bitstream *stream, long size, s_synchronous_gamestate_message *message)
{
	byte *data = message->data;
	message->unknown00 = function_1959c0(stream, 23);
	message->size = function_1959c0(stream, 13);
	bool valid = !stream_overflowed(stream) && message->unknown00 >= 0 && message->size >= 0 && message->size <= 0xffff && size == message->size + 8;
	if (valid && message->size > 0)
	{
		function_195820(stream, data, message->size * 8);
		if (stream_overflowed(stream))
			valid = false;
		else
			valid = true;
	}
	return valid;
}

// @retail 0xb2b30
void network_message_types_register_synchronous(c_type_659ceb *collection)
{
	collection->function_x5c51c9(_network_message_type_synchronous_update, "synchronous-update", 0, 0x4048, 0x4048,
		(t_message_encode)function_b2710, (t_message_decode)function_b2730, (t_message_compare)function_b2770);
	collection->function_x5c51c9(_network_message_type_synchronous_actions, "synchronous-actions", 0, 0x180, 0x180,
		(t_message_encode)function_b2790, (t_message_decode)function_b2860, (t_message_compare)function_b2920);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_synchronous_join, "synchronous-join", 4, function_b2980, function_b29a0);
	collection->function_x5c51c9(_network_message_type_synchronous_gamestate, "synchronous-gamestate", 1, 8, 0xffff,
		(t_message_encode)function_b29e0, (t_message_decode)function_b2a90, NULL);
}

// @retail 0xb2c50
void __stdcall function_b2c50(s_bitstream *stream, long size, s_game_results_message *message)
{
	function_1955d0(stream, &message->unknown00, 32);
	function_1955d0(stream, &message->unknown04, 32);
	function_197680(stream, message->results);
}

// @retail 0xb2c80
bool __stdcall function_b2c80(s_bitstream *stream, long size, s_game_results_message *message)
{
	function_195820(stream, &message->unknown00, 32);
	function_195820(stream, &message->unknown04, 32);
	if (function_197d80(stream, message->results))
		return true;
	return false;
}

// @retail 0xb2cc0
void network_message_types_register_game_results(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_game_results, "game-results", 0x4fb8, function_b2c50, function_b2c80);
}

// @retail 0xb2d10
void __stdcall function_b2d10(s_bitstream *stream, long size, s_test_message *message)
{
	stream_write_bit(stream, message->flag);
	stream_write_checked(stream, message->value, 16);
}

// @retail 0xb2da0
bool __stdcall function_b2da0(s_bitstream *stream, long size, s_test_message *message)
{
	message->flag = function_1957d0(stream);
	message->value = function_1959c0(stream, 16);
	bool ok = !stream_overflowed(stream);
	return ok;
}

__declspec(noinline) void network_message_types_register_test(c_type_659ceb *collection);

// @retail 0xb2de0
void network_message_types_register_test(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_test, "test", 8, function_b2d10, function_b2da0);
}
