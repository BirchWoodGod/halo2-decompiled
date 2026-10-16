#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"
#include "data_array.h"
#include "game_engine_events.h"
#include "object_default_placement.h"
#include <math.h>
#include <string.h>

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_19CA10.CPP: the multiplayer globals' marker pairs: for each of the
   16 keys, the first markers of types 7 and 8 that carry it, and a message to
   a player */

/* the players (0x21c bytes each) as the marker pairs see them */
struct s_marker_player
{
	short identifier;
	word flags0 : 2;
	word flag2 : 1;
	word flags3 : 4;
	word near_marker : 1;
	word flag8 : 1;
	word flags9 : 7;
	byte unknown04[0x28 - 4];
	short local_index;
	byte unknown2a[0x2c - 0x2a];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_message_view
{
	byte unknown00[4];
	real delay;
	byte unknown08[0xf8 - 8];
	long string_handle;
};

struct s_marker_runtime_view
{
	byte unknown00[0x534];
	s_message_view *message;
};

struct s_marker_globals_definition
{
	byte unknown00[8];
	long runtime_count;
	s_marker_runtime_view *runtime;
};

void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_24cbee(long player_index, word const *text);

/* shows a player the multiplayer globals' message */
// @retail 0x19ca10
void function_19ca10(long player_index)
{
	s_marker_player *player;
	s_marker_globals_definition *definition;
	word text[0x100];

	text[0] = 0;
	player = (s_marker_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_marker_player));
	wcsncpy(text, L"", 0xff);
	text[0xff] = 0;
	definition = (s_marker_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes;
	if (definition->runtime_count)
	{
		long string_handle = definition->runtime->message->string_handle;

		text[0] = 0;
		if (g_510c94 && g_510c94->string_list != NONE)
			function_1a0180(g_510c94->string_list, string_handle, text);
	}
	function_24cbee(player->local_index, text);
}

// @retail 0x19cb20
bool function_19cb20(s_marker_pair *pair, long key)
{
	long index;

	index = NONE;
	function_19ec40(NULL, 0.0f, 7, NONE, (short)key, 1, &index, 0.0f);
	pair->first = index;
	index = NONE;
	function_19ec40(NULL, 0.0f, 8, NONE, (short)key, 1, &index, 0.0f);
	pair->second = index;
	pair->unknown08 = 0;
	pair->unknown0c = 0;

	return pair->first != NONE && pair->second != NONE;
}

// @retail 0x19cad0
void function_19cad0()
{
	s_mp_globals *globals = g_4e9ae8;
	long key;

	globals->marker_pair_count = 0;
	for (key = 0; key < 16; key++)
	{
		if (function_19cb20(&globals->marker_pairs[globals->marker_pair_count], key))
			globals->marker_pair_count++;
	}
}

/* the iterator of 0x19f300 (the players with a unit) */
struct s_marker_player_iterator
{
	s_marker_player *player;
	s_record_pool *data;
	long datum_index;
	long index;
};

struct s_marker_object
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x70 - 0x18];
	point3f position;
};

struct s_marker_object_header
{
	byte unknown00[8];
	s_marker_object *object;
};

bool player_iterator_next_with_unit(long *iterator);
real distance_sq3f(point3f const *a, point3f const *b);

real const g_45dc20 = 0.25f;

// @retail 0x19ce10
void function_19ce10(s_marker_pair *pair)
{
	if (!pair->unknown08)
	{
		s_marker_entry const &entry = g_4e0350->marker_entries[pair->first];
		point3f position = entry.position;
		s_marker_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			s_marker_player *player = iterator.player;
			long unit_index = player->unit_index;
			point3f unit_position;
			vector3f delta;

			function_b9dd0(unit_index, &unit_position);
			vector3d_from_points3d(&position, &unit_position, &delta);
			if (((s_marker_object_header *)g_4e0300->data)[unit_index & 0xffff].object->parent_index == NONE &&
				delta.j * delta.j + delta.i * delta.i + delta.k * delta.k < g_45dc20)
			{
				player->near_marker = true;
			}
		}
	}
}

struct s_shapes;
struct s_shape_result;
void __stdcall function_df5f0(long object_index, point3f *center, real *height, real *radius);
bool __stdcall function_16a440(dword flags, point3f const *position, real extent, real height, real radius,
	long ignore_object, long ignore_parent, s_shapes *shapes);
bool function_245ef0(s_shapes const *shapes, point3f const *point, s_shape_result *result);
void __stdcall function_a7840(short player_index, dword mask);

// @retail 0x19cb90
bool function_19cb90(s_marker_pair *pair, long player_index)
{
	bool result = false;
	long const *player_reference = &player_index;
	s_marker_player *player = (s_marker_player *)(g_4e8c24->data + (*player_reference & 0xffff) * sizeof(s_marker_player));
	s_marker_entry const &selected = g_4e0350->marker_entries[pair->second];
	point3f destination = selected.position;
	point3f center;
	real radius;
	real height;
	function_df5f0(player->unit_index, &center, &height, &radius);
	real extent = height * 0.5f + radius;
	point3f origin = destination;
	origin.z += extent;
	byte shapes_storage[0xb808];
	if (function_16a440(0x500038, &origin, extent, height, radius, player->unit_index, NONE, (s_shapes *)shapes_storage))
	{
	point3f point = destination;
	point.z += radius;
	byte result_storage[0x30];
	if (function_245ef0((s_shapes *)shapes_storage, &point, (s_shape_result *)result_storage))
	{
	long object_index = *(long *)result_storage;
	if (object_index != NONE)
	{
		byte *object = (byte *)((s_marker_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		if ((1 << object[0xaa]) & 3)
		{
			long hit_player_index = *(long *)(object + 0x13c);
			if (hit_player_index != NONE)
			{
				byte *other = g_4e8c24->data + (hit_player_index & 0xffff) * sizeof(s_marker_player);
				((s_marker_player *)other)->flag2 = true;
				function_a7840(*(short *)(object + 0x13c), 8);
			}
		}
	}
	result = true;
	}
	}
	return result;
}

void marker_get_position_and_angle(long index, point3f *position, real *angle);
real function_30bf0(vector3f *vector);
void function_1874b0(long player_index, vector3f const *forward);
void function_a8aa0(long unit_index);

// @retail 0x19cf20
void __stdcall function_19cf20(s_marker_pair *pair)
{
	(void)&pair;
	if (!pair->unknown08)
	{
		real closest_distance = g_45dc20;
		s_marker_entry const &entry = g_4e0350->marker_entries[pair->first];
		point3f position = entry.position;
		real first_angle = *(real *)entry.unknown0c;
		long closest_player = NONE;
		s_marker_player_iterator iterator;
		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			long unit_index = iterator.player->unit_index;
			point3f unit_position;
			function_b9dd0(unit_index, &unit_position);
			vector3f delta;
			vector3d_from_points3d(&position, &unit_position, &delta);
			real distance = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
			if (((s_marker_object_header *)g_4e0300->data)[unit_index & 0xffff].object->parent_index == NONE &&
				closest_distance > distance)
			{
				closest_player = iterator.datum_index;
				closest_distance = distance;
			}
		}
		if (closest_player != NONE)
		{
			s_marker_player *player = (s_marker_player *)(g_4e8c24->data + (closest_player & 0xffff) * sizeof(s_marker_player));
			if (!(*(word *)((byte *)player + 2) & 0x100))
			{
				if (!function_19cb90(pair, closest_player))
				{
					s_marker_object *object = ((s_marker_object_header *)g_4e0300->data)[player->unit_index & 0xffff].object;
					vector3f forward = *(vector3f *)&object->position;
					point3f destination;
					real second_angle;
					marker_get_position_and_angle(pair->second, &destination, &second_angle);
					player->flag8 = true;
					s_event event;
					game_engine_event_initialize_inline(&event, 0, 0x16);
					event.cause_player_index = closest_player;
					event.a = closest_player;
					function_19eb90(&event);
					real angle = (real)atan2(forward.j, forward.i) + second_angle - first_angle;
					forward.i = (real)cos(angle);
					forward.j = (real)sin(angle);
					forward.k = 0.0f;
					function_30bf0(&forward);
					function_b9a50(player->unit_index);
					function_b75a0(player->unit_index, &destination, &forward, g_4687b0, NULL, false);
					if (player->local_index != NONE)
						function_1874b0(player->local_index, &forward);
					s_marker_globals_definition *definition = (s_marker_globals_definition *)g_4e3b44[g_4e034c->index & 0xffff].bytes;
					real delay = g_510c54->field_2_3 * definition->runtime->message->delay;
					long ticks;
					__asm
					{
						fld delay
						fistp ticks
					}
					pair->unknown08 = ticks;
				}
				else
				{
					player = (s_marker_player *)(g_4e8c24->data + (closest_player & 0xffff) * sizeof(s_marker_player));
					if (pair->unknown0c > 0)
						pair->unknown0c--;
					else
					{
						pair->unknown0c = g_510c54->field_2_3 * 4;
						if (player->local_index != NONE)
							function_19ca10(closest_player);
						function_a8aa0(player->unit_index);
					}
					return;
				}
			}
		}
		pair->unknown0c = 0;
	}
	else
		pair->unknown08--;
}

// @retail 0x19cd10
void function_19cd10(void)
{
	s_record_pool *data = g_4e8c24;
	s_marker_player_iterator iterator;
	iterator.data = data;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
		iterator.player->near_marker = false;
	s_mp_globals *globals = g_4e9ae8;
	for (long i = 0; i < globals->marker_pair_count; i++)
		function_19ce10(&globals->marker_pairs[i]);
	iterator.data = data;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
	{
		if (!(*(word *)((byte *)iterator.player + 2) & 0x80))
			iterator.player->flag8 = false;
	}
	for (long j = 0; j < globals->marker_pair_count; j++)
	{
		function_19cf20(&globals->marker_pairs[j]);
		globals = g_4e9ae8;
	}
}
