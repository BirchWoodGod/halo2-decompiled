// @flags /O2 /Gr
/* UNKNOWN_19D220.CPP: game variant defaults and checks (lane H) */

#include "unknown_11c920.h"
#include <string.h>
#include <wchar.h>
#include "globals.h"
#include "unknown_19d220.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

void function_1a0180(long tag_index, long string_handle, word *buffer);

/* the multiplayer globals tag the globals tag refers to: its first block
   element holds the string list of the default variant names */
struct s_multiplayer_globals_element
{
	byte unknown00[0x1c];
	long string_list_tag_index;
};

struct s_multiplayer_globals
{
	long count;
	s_multiplayer_globals_element *elements;
};

/* fills a variant with the defaults of a variant type (0 slayer, 1 oddball,
   2 juggernaut, 3 king of the hill, 4 capture the flag, 5 assault,
   6 territories) */
__forceinline s_game_variant *variant_defaults_initialize(s_game_variant *variant, long type)
{
	s_game_variant result;
	long engine = 2;
	long string_handle = 0;
	word name[0x100];

	memset(&result, 0, sizeof(result));
	switch (type)
	{
	case 0:
		engine = 2;
		string_handle = 0xe000780;
		break;
	case 2:
		engine = 7;
		string_handle = 0x12000783;
		break;
	case 3:
		engine = 4;
		string_handle = 0xc000782;
		break;
	case 1:
		engine = 3;
		string_handle = 0xf000781;
		break;
	case 4:
		engine = 1;
		string_handle = 0xb00077f;
		break;
	case 5:
		engine = 9;
		string_handle = 0xf000785;
		break;
	case 6:
		engine = 8;
		string_handle = 0x13000784;
		break;
	}
	name[0] = 0;
	if (g_4e034c && g_4e034c->index != NONE)
	{
		s_multiplayer_globals_element *elements = ((s_multiplayer_globals *)g_4e3b44[g_4e034c->index & 0xffff].bytes)->elements;

		if (elements->string_list_tag_index != NONE)
		{
			function_1a0180(elements->string_list_tag_index, string_handle, name);
		}
	}
	wcsncpy(result.name, name, 0x1f);
	result.name[0x1f] = 0;
	result.field_xcb8724 = engine;
	result.unknown03 = -1;
	if (type >= 4 && type <= 6)
	{
		result.flags48 |= 1;
	}
	else
	{
		result.flags48 &= ~1;
	}
	result.flags48 = (result.flags48 & 0xffff8f9b) | 0xf9a;
	result.unknown4c = 0;
	switch (type)
	{
	case 0:
		result.unknown50 = 0x19;
		break;
	case 2:
		result.unknown50 = 0xf;
		break;
	case 3:
		result.unknown50 = 0x78;
		break;
	case 1:
		result.unknown50 = 0x78;
		break;
	case 6:
		result.unknown50 = 0x12c;
		break;
	case 4:
		result.unknown50 = 3;
		break;
	case 5:
		result.unknown50 = 3;
		break;
	}
	result.unknown54 = 0x1e0;
	result.unknown58 = 0;
	result.unknown74 = 0x10;
	result.unknown78 = 0x10;
	result.unknown7c = 0;
	result.unknown80 = type >= 4 && type <= 6 ? 10 : 5;
	result.unknown84 = type >= 4 && type <= 6 ? 10 : 5;
	result.unknown88 = 0;
	result.unknowna4 = 0;
	result.unknowna8 = 2;
	result.unknownac = 10;
	result.unknownb0 = 0;
	result.unknowncc = 0;
	result.unknowncd = 0;
	result.unknownce = 0;
	result.unknowncf = 0;
	result.unknownd0 = 0;
	result.unknownd1 = 0;
	result.unknownd2 = 0;
	result.unknownd3 = 0;
	result.unknownd4 = 0;
	result.unknownd5 = 0;
	result.unknownd6 = 0;
	result.unknownd7 = 0;
	switch (type)
	{
	case 0:
		result.engine_flags = (result.engine_flags & ~5) | 2;
		break;
	case 2:
		result.king.unknownf4 = 2;
		result.king.flags = (result.king.flags & ~0x24) | 0x1b;
		break;
	case 3:
		result.king.flags &= ~0x1f;
		result.king.unknownf4 = 0x3c;
		break;
	case 1:
		result.ball.flags &= ~7;
		result.ball.unknownf4 = 1;
		result.ball.unknownf6 = 0;
		result.ball.unknownf8 = 0;
		result.ball.unknownfa = 0;
		break;
	case 5:
		result.flag.flags = (result.flag.flags & ~0xdc) | 0x22;
		result.flag.unknownfc = 0;
		result.flag.unknown108 = 5;
		result.flag.unknown10a = 3;
		result.flag.unknownf4 = 0x1e;
		result.flag.unknownf8 = 0;
		result.flag.unknown104 = 0;
		result.flag.unknown100 = 1;
		break;
	case 4:
		result.flag.flags = (result.flag.flags & ~0xfc) | 2;
		result.flag.unknownfc = 0;
		result.flag.unknown108 = 0;
		result.flag.unknown10a = 0;
		result.flag.unknownf4 = 0x1e;
		result.flag.unknownf8 = 0;
		result.flag.unknown104 = 0;
		result.flag.unknown100 = 0;
		break;
	case 6:
		result.territories.unknownf4 = 5;
		result.territories.unknownf0 = 3;
		result.territories.unknownf2 = 5;
		break;
	}
	*variant = result;
	return variant;
}

__declspec(noinline) s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type);

// @retail 0x19d220
s_game_variant *__stdcall function_19d220(s_game_variant *variant, long type)
{
	return variant_defaults_initialize(variant, type);
}

/* clamps a setting into its range in place */
static inline void pin_value(long *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(short *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

static inline void pin_value(char *value, long lower, long upper)
{
	*value = PIN(*value, lower, upper);
}

/* the settings of capture the flag and assault */
static __forceinline void flag_settings_verify(s_game_variant_flag_settings *settings)
{
	settings->flags &= 0xff;
	pin_value(&settings->unknownf4, 0, 0x7fff);
	pin_value(&settings->unknownf8, 0, 2);
	pin_value(&settings->unknownfc, 0, 1);
	pin_value(&settings->unknown100, 0, 3);
	pin_value(&settings->unknown104, 0, 2);
}

/* the settings of king of the hill and juggernaut */
static __forceinline void king_settings_verify(s_game_variant_king_settings *settings, dword flags_mask, long maximum)
{
	settings->flags &= flags_mask;
	pin_value(&settings->unknownf4, 0, maximum);
}

/* clamps every setting of a variant into its range; a variant that needed
   any change is replaced by the slayer defaults */
// @retail 0x19d650
bool function_19d650(s_game_variant *variant)
{
	bool valid = true;
	s_game_variant original = *variant;

	variant->flags &= 1;
	variant->name[0x1f] = 0;
	variant->flags48 &= 0x7fff;
	variant->unknown4c = PIN(variant->unknown4c, 0, 6);
	pin_value(&variant->unknown50, 0, 0x7fff);
	pin_value(&variant->unknown54, 0, 0x7fff);
	pin_value(&variant->unknown58, 0, 2);
	pin_value(&variant->unknown74, 0, 0x10);
	pin_value(&variant->unknown78, 0, 0x10);
	pin_value(&variant->unknown7c, 0, 0x7fff);
	pin_value(&variant->unknown80, 0, 0x7fff);
	pin_value(&variant->unknown84, 0, 0x7fff);
	pin_value(&variant->unknown88, 0, 2);
	pin_value(&variant->unknowna4, 0, 2);
	pin_value(&variant->unknowna8, 0, 2);
	pin_value(&variant->unknownac, 0, 0x7fff);
	variant->unknownb0 = 0;
	variant->unknownb4 = PIN(variant->unknownb4, 0, 8);
	pin_value(&variant->unknowncc, 0, 3);
	pin_value(&variant->unknowncd, 0, 7);
	pin_value(&variant->unknownce, 0, 7);
	pin_value(&variant->unknowncf, 0, 4);
	pin_value(&variant->unknownd0, 0, 4);
	pin_value(&variant->unknownd1, 0, 4);
	pin_value(&variant->unknownd2, 0, 6);
	pin_value(&variant->unknownd3, 0, 6);
	pin_value(&variant->unknownd4, 0, 0x13);
	pin_value(&variant->unknownd5, 0, 3);
	pin_value(&variant->unknownd6, 0, 0x14);
	pin_value(&variant->unknownd7, 0, 0x14);
	pin_value(&variant->field_xcb8724, 1, 9);
	if (variant->field_xcb8724 == original.field_xcb8724)
	{
		switch (variant->field_xcb8724)
		{
		case 9:
			pin_value(&variant->flag.unknown108, 0, 0xf);
			pin_value(&variant->flag.unknown10a, 0, 0xf);
		case 1:
			variant->flags48 |= 1;
			flag_settings_verify(&variant->flag);
			break;
		case 2:
			variant->engine_flags &= 7;
			break;
		case 3:
			variant->ball.flags &= 7;
			pin_value(&variant->ball.unknownf4, 0, 3);
			pin_value(&variant->ball.unknownf6, 0, 1);
			pin_value(&variant->ball.unknownf8, 0, 2);
			pin_value(&variant->ball.unknownfa, 0, 3);
			break;
		case 4:
			king_settings_verify(&variant->king, 0x1f, 0x7fff);
			break;
		case 7:
			variant->flags48 &= ~1;
			king_settings_verify(&variant->king, 0x7f, 2);
			break;
		case 8:
			pin_value(&variant->territories.unknownf0, 1, 8);
			pin_value(&variant->territories.unknownf2, 1, 0x7fff);
			pin_value(&variant->territories.unknownf4, 1, 0x7fff);
			break;
		default:
			variant->field_xcb8724 = 10;
			break;
		}
	}
	if (memcmp(&original, variant, sizeof(original)) != 0)
	{
		variant_defaults_initialize(variant, 0);
		valid = false;
	}
	return valid;
}

/* whether the variant is valid as it is: checks a copy */
// @retail 0x19d620
bool function_19d620(s_game_variant *variant)
{
	s_game_variant copy = *variant;

	return function_19d650(&copy);
}
