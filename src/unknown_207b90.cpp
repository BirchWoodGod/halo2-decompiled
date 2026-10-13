// @flags /O2 /Gr
/* UNKNOWN_207B90.CPP: creation and scenario placement callbacks.
   See docs/unknown_207b90.md for the original-object mapping. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

struct s_sound_scenery_object_view
{
	long definition_index;
	dword object_flags; /* bit 16: no shadow */
	byte unknown008[0x12c - 8];
	dword value_12c;
	dword value_130;
	dword value_134;
	dword value_138;
	dword value_13c;
	dword value_140;
	dword value_144;
};

struct s_sound_scenery_header_view
{
	byte unknown00[8];
	s_sound_scenery_object_view *object;
};

struct s_scenario_sound_scenery_view
{
	byte unknown00[0x34];
	dword value_34;
	dword value_38;
	dword value_3c;
	dword value_40;
	dword value_44;
	dword value_48;
	dword value_4c;
};

#define SOUND_SCENERY_GET(index) (((s_sound_scenery_header_view *)g_4e0300->data)[(index) & 0xffff].object)

// @retail 0x207b90
bool __stdcall function_207b90(long object_index, long creation_argument1, long creation_argument2)
{
	s_sound_scenery_object_view *scenery = SOUND_SCENERY_GET(object_index);
	dword *flags = &scenery->object_flags;
	*flags |= 0x10000;
	return true;
}

// @retail 0x207bc0
void __stdcall function_207bc0(long object_index, s_scenario_sound_scenery_view *placement)
{
	s_sound_scenery_object_view *scenery = SOUND_SCENERY_GET(object_index);
	scenery->value_12c = placement->value_34;
	scenery->value_130 = placement->value_38;
	scenery->value_134 = placement->value_3c;
	scenery->value_138 = placement->value_40;
	scenery->value_13c = placement->value_44;
	scenery->value_140 = placement->value_48;
	scenery->value_144 = placement->value_4c;
}

/* Actual sound scenery type-definition prefix at 0x4680b8, through
   creation and placement. Later fields and parent types are outside
   this view. Retail's creation ABI has three stack arguments. */
struct s_sound_scenery_type_definition_view
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[7];
	bool (__stdcall *create)(long, long, long);
	void (__stdcall *place)(long, s_scenario_sound_scenery_view *);
};

s_sound_scenery_type_definition_view g_4680b8 =
{
	"sound_scenery", 'ssce', 0x148, 0xd8, 0xe0, 0x50,
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL },
	function_207b90,
	function_207bc0
};
