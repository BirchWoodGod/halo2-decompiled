// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1502E0.CPP: player helpers at the end of the players code (target
   candidates, the local players' view state, the players' census) */

#include "unknown_11c920.h"
#include "segment_sphere.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <string.h>
#include "object_iterator.h"

/* the players (g_4e8c24, 0x21c bytes each) */
struct s_tail_player
{
	short salt;
	word flags;
	byte unknown04[0x10 - 4];
	long leave_time;
	byte machine_address[6];
	short machine_index;
	short machine_user;
	byte unknown1e[2];
	long machine_controller;
	long controller_index;
	short local_index;
	byte unknown2a[2];
	long unit_index;
	long previous_unit_index;
	byte unknown34[0x174 - 0x34];
	long object_index174;
	byte unknown178[0x21c - 0x178];
};

/* the object header data (g_4e0300, 12 bytes each) */
struct s_tail_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	void *object;
};

/* the objects as the players code sees them */
struct s_tail_object
{
	byte unknown00[0xaa];
	byte type;
};

/* a candidate: its priority and its object */
struct s_target_candidate
{
	short priority;
	byte flags;
	byte unknown03;
	long object_index;
};

/* g_4e8c20 seen as the players' census */
struct s_player_census
{
	byte unknown00[4];
	bool all_without_units;
	bool any_available;
};

/* the view entries of g_4ed284 (0x94 bytes from +0x14) */
struct s_view_entry
{
	byte unknown00[0x28];
	short index28;
	byte unknown2a[0x8e - 0x2a];
	bool flag8e;
	bool flag8f;
	byte unknown90[0x94 - 0x90];

	__forceinline void clear_flags()
	{
		if (flag8e)
			flag8f = true;
		index28 = NONE;
	}
};

struct s_view_globals
{
	byte unknown00[0x14];
	s_view_entry entries[4];
};

struct s_object;
s_object *function_bae20(long object_index, dword type_mask);
point3f *function_b9dd0(long object_index, point3f *result);

static inline s_tail_player *tail_player_get(long player_index)
{
	return (s_tail_player *)g_4e8c24->data + (player_index & 0xffff);
}

static inline s_tail_object *tail_object_get(long object_index)
{
	return (s_tail_object *)((s_tail_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

void function_14cad0(long player_index, long unit_index);
void function_187a60(long datum_index);
void function_152340(void);

// @retail 0x151320
bool function_151320(s_target_candidate *candidate, s_target_candidate *best, long player_index)
{
	bool result = false;
	long const *player_reference = &player_index;

	if (candidate->priority <= best->priority)
	{
		point3f player_position;
		point3f best_position;
		point3f candidate_position;
		vector3f to_candidate;
		vector3f to_best;

		if (candidate->priority != best->priority)
		{
			goto done;
		}
		function_b9dd0(tail_player_get(*player_reference)->unit_index, &player_position);
		function_b9dd0(best->object_index, &best_position);
		function_b9dd0(candidate->object_index, &candidate_position);
		vector3d_from_points3d(&player_position, &candidate_position, &to_candidate);
		vector3d_from_points3d(&player_position, &best_position, &to_best);
		real candidate_distance = to_candidate.j * to_candidate.j + to_candidate.i * to_candidate.i + to_candidate.k * to_candidate.k;
		real best_distance = length_sq3f(&to_best);
		if (!(candidate_distance < best_distance))
		{
			result = false;
			goto done;
		}
	}
	*best = *candidate;
	result = true;
done:
	return result;
}


static __forceinline bool player_candidate_has_type(s_target_candidate *candidate, dword type_mask)
{
	bool result = false;
	long object_index = candidate->object_index;

	if (function_bae20(object_index, NONE))
	{
		s_tail_object_header *header = (s_tail_object_header *)g_4e0300->data + (object_index & 0xffff);

		result = (type_mask & (1 << header->type)) != 0;
	}
	return result;
}

__declspec(noinline) bool function_1518f0(s_target_candidate *candidate, dword type_mask);

// @retail 0x1518f0
bool function_1518f0(s_target_candidate *candidate, dword type_mask)
{
	return player_candidate_has_type(candidate, type_mask);
}

PRIVATE __forceinline void local_player_view_clear(s_view_globals *views, short index)
{
	views->entries[index].clear_flags();
}

// @retail 0x1520f0
void function_1520f0(long player_index)
{
	s_tail_player const volatile *player = tail_player_get(player_index);
	short local_index = player->local_index;

	if (local_index != NONE)
		local_player_view_clear((s_view_globals *)g_4ed284, local_index);
}

// @retail 0x152340
void function_152340(void)
{
	s_player_census *census = (s_player_census *)g_4e8c20;
	s_record_pool_iterator iterator;
	s_tail_player *player;

	census->all_without_units = true;
	census->any_available = false;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		if (player->unit_index != NONE)
		{
			census->all_without_units = false;
		}
		else if (player->flags & 8)
		{
			census->all_without_units = false;
		}
		else
		{
			census->any_available = true;
		}
	}
}

/* the game options' players (16 of 0xe4 bytes at +0x2dc): an identifier
   and two sets of traits */
struct s_tail_traits
{
	byte unknown00[0x1c];
};

struct s_tail_options_player
{
	byte unknown00[0xe];
	byte identifier[12];
	byte unknown1a[0xac - 0x1a];
	s_tail_traits traits[2];
};

struct s_tail_options
{
	byte unknown00[0x2dc];
	s_tail_options_player players[16];
};

struct s_tail_scenario_entry
{
	byte unknown00[0x2c];
	short value2c;
};

struct s_tail_scenario
{
	byte unknown00[0x100];
	long entry_count;
	s_tail_scenario_entry *entries;
};

struct s_tail_player_identifier
{
	byte unknown00[4];
	byte identifier[12];
};

// @retail 0x1529a0
s_tail_traits *function_1529a0(long player_index)
{
	s_tail_player_identifier *player = (s_tail_player_identifier *)tail_player_get(player_index);
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	bool alternate = false;
	s_tail_options_player *entry;
	long i;

	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
	{
		alternate = true;
	}
	entry = ((s_tail_options *)g_4e6948)->players;
	for (i = 0; i < 16; i++, entry++)
	{
		if (memcmp(player->identifier, entry->identifier, sizeof(entry->identifier)) == 0)
		{
			return &entry->traits[alternate ? 1 : 0];
		}
	}
	return 0;
}

/* the units and their definitions as the census of 0x152580 reads them */
struct s_tail_unit
{
	long definition_index;
	byte unknown04[0x184 - 4];
	real value184;
	byte unknown188[0x22a - 0x188];
	short value22a;
	short value22c;
};

struct s_tail_seat
{
	byte unknown00[0x88];
	byte value88;
	byte unknown89[0x90 - 0x89];
	long tag90;
	byte unknown94[4];
	long tag98;
};

struct s_tail_unit_definition
{
	byte unknown00[0x1fc];
	byte value1fc;
	byte unknown1fd[0x2c0 - 0x1fd];
	long count2c0;
	byte unknown2c4[0x2d0 - 0x2c4];
	long seat_count;
	s_tail_seat *seats;
};

struct s_tail_seat_definition
{
	byte unknown00[0x128];
	byte value128;
};

struct s_tail_unit_state
{
	short value0;
	short value2;
	short value4;
	short value6;
};

// @retail 0x152580
void function_152580(long unit_index, s_tail_unit_state *state)
{
	s_tail_unit *unit = (s_tail_unit *)((s_tail_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
	s_tail_unit_definition *definition = (s_tail_unit_definition *)g_4e3b44[unit->definition_index & 0xffff].bytes;
	long value = 0;
	real scaled;
	long rounded;

	if (definition->seat_count > 0)
	{
		s_tail_seat *seat = definition->seats;

		if (seat->tag90 != NONE)
		{
			value = ((s_tail_seat_definition *)g_4e3b44[seat->tag90 & 0xffff].bytes)->value128;
		}
		else if (seat->tag98 != NONE)
		{
			value = seat->value88;
		}
	}
	if (!value)
	{
		value = definition->value1fc;
	}
	state->value0 = (short)value;
	scaled = (1.0f - unit->value184) * 100.0f;
	__asm
	{
		fld scaled
		fistp rounded
	}
	state->value6 = (short)rounded;
	if (definition->count2c0 > 0)
	{
		state->value2 = unit->value22a;
		state->value4 = unit->value22c;
	}
}

/* a horizontal forward vector and its left */
struct s_horizontal_axes
{
	vector3f forward;
	vector3f left;
};

real function_30bf0(vector3f *v);

static __forceinline void horizontal_left_set(real i, real j, vector3f *left)
{
	left->i = 0.0f - j;
	left->j = i;
	left->k = 0.0f;
}

// @retail 0x1502e0
void function_1502e0(vector3f const *forward, vector3f const *fallback, s_horizontal_axes *axes)
{
	axes->forward = *forward;
	axes->forward.k = 0.0f;
	if (function_30bf0(&axes->forward) == 0.0f)
	{
		axes->forward = *fallback;
		axes->forward.k = 0.0f;
		if (function_30bf0(&axes->forward) == g_45dbd8)
		{
			axes->forward = *g_4687a8;
		}
	}
	horizontal_left_set(axes->forward.i, axes->forward.j, &axes->left);
}

/* an object is being deleted: the players forget it (one of the object
   deletion callbacks, g_468664) */
// @retail 0x152cf0
void __stdcall function_152cf0(long object_index)
{
	if ((1 << tail_object_get(object_index)->type) & 3)
	{
		s_record_pool_iterator iterator;
		s_tail_player *player;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
		{
			if (player->unit_index == object_index)
			{
				s_tail_player *owner = tail_player_get(iterator.datum_index);

				owner->previous_unit_index = owner->unit_index;
				function_14cad0(iterator.datum_index, NONE);
				function_152340();
			}
			if (player->object_index174 == object_index)
			{
				player->object_index174 = NONE;
			}
			if (player->previous_unit_index == object_index)
			{
				player->previous_unit_index = NONE;
			}
		}
	}
	function_187a60(object_index);
}

long __stdcall function_cdeb0(long unit_index, long state_name, long a, long b);
long function_cbe60(long unit_index);
bool function_cd660(long unit_index);

// @retail 0x150dd0
bool function_150dd0(long player_index)
{
	s_tail_player *player = tail_player_get(player_index);
	if ((short)function_cdeb0(player->unit_index, NONE, NONE, 0) == NONE)
		return true;
	if (g_4e6948->state != 2)
	{
		long unit_index = player->unit_index;
		if (function_cbe60(unit_index) == NONE && !function_cd660(unit_index))
			return true;
	}
	return false;
}

struct s_player_motion_unit
{
	byte unknown00[0x150];
	vector3f vector150;
	vector3f vector15c;
	byte unknown168[0x180 - 0x168];
	vector3f vector180;
};

struct s_player_motion_control
{
	byte unknown00[0x28];
	vector3f vector28;
	vector3f vector34;
	vector3f vector40;
	byte unknown4c[0x7c - 0x4c];
};

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);

// @retail 0x150790
void function_150790(long unit_index)
{
	s_player_motion_control control;
	s_player_motion_unit *unit = (s_player_motion_unit *)tail_object_get(unit_index);
	function_c6ef0((s_unit_state_c6ef0 *)&control);
	control.vector28 = unit->vector150;
	control.vector34 = unit->vector15c;
	control.vector40 = unit->vector180;
	function_c6de0(unit_index, &control);
}

struct s_player_target_object
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x168 - 0x40];
	vector3f direction;
};

void function_cafc0(long unit_index, point3f *position);
bool function_1078f0(long control_index, vector3f const *direction);
bool function_107870(long device_index);

// @retail 0x1516c0
void function_1516c0(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_target_object *object = (s_player_target_object *)tail_object_get(object_index);
	point3f eye;
	function_cafc0(player->unit_index, &eye);
	if (function_11e5e0(&eye, &unit->direction, &object->position, object->radius) &&
		function_1078f0(object_index, &unit->direction) && function_107870(object_index))
	{
		s_target_candidate candidate;
		memset(&candidate, 0, sizeof(candidate));
		candidate.priority = 3;
		candidate.object_index = object_index;
		function_151320(&candidate, *best_reference, *player_reference);
	}
}

struct s_player_pickup_object
{
	long definition_index;
	byte unknown04[0x30 - 4];
	point3f position;
	byte unknown3c[0x12c - 0x3c];
	dword flags;
	byte unknown130[0x14c - 0x130];
	long field_14c;
};

struct s_player_pickup_definition
{
	byte unknown00[0x290];
	short type;
};

bool __stdcall function_cd7b0(long unit_index, long weapon_index, bool *modes);

// @retail 0x151780
void function_151780(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_object *unit = (s_player_target_object *)tail_object_get(player->unit_index);
	s_player_pickup_object *object = (s_player_pickup_object *)tail_object_get(object_index);
	s_player_pickup_definition *definition = (s_player_pickup_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
	bool special = definition->type != 0;
	real radius = unit->radius + (special ? 0.1f : 0.0f) + 0.3f;
	if (!(object->flags & 1) && object->field_14c != player->unit_index)
	{
		vector3f delta;
		vector3d_from_points3d(&unit->position, &object->position, &delta);
		real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
		bool modes[4];
		if (distance < radius * radius && function_cd7b0(player->unit_index, object_index, modes))
		{
			bool first = modes[0];
			bool second = modes[1];
			bool third = modes[2];
			bool fourth = modes[3];
			long flags = 0;
			if (first)
				flags = 1;
			else if (third)
				flags = 2;
			if (second)
				flags |= 4;
			else if (fourth)
				flags |= 8;
			s_target_candidate candidate;
			memset(&candidate, 0, sizeof(candidate));
			candidate.object_index = object_index;
			candidate.flags = (byte)flags;
			candidate.priority = special ? 7 : 1;
			function_151320(&candidate, *best_reference, *player_reference);
		}
	}
}

struct s_player_removed_unit
{
	byte unknown00[0x10a];
	word flags;
	byte unknown10c[0x2a8 - 0x10c];
	long player_index;
};

struct s_player_census_counts
{
	long count;
	byte unknown04[4];
	short user_count;
	short controller_count;
	long users[4];
	long controllers[4];
};

extern byte g_4cf77b;

class c_class_6a600;
void function_69580(c_class_6a600 *world, long player_index);
void function_b7360(long object_index);
void player_control_set_unit(long user_index, long unit_index);
void function_1682bf(long unit_index, long user_index, long character_index);
void function_157de0(long player_index);

// @retail 0x152df0
void function_152df0(long player_index)
{
	long const *player_reference = &player_index;
	s_tail_player *player = tail_player_get(*player_reference);
	if (player->unit_index != NONE)
	{
		s_player_removed_unit *unit = (s_player_removed_unit *)tail_object_get(player->unit_index);
		unit->flags |= 0x400;
		function_b7360(player->unit_index);
	}
	if (g_4e6948->mode == 4)
	{
		struct
		{
			s_player_removed_unit *unit;
			s_type_f1af8e iterator;
		} state;
		state.iterator.signature = 0x86868686;
		state.iterator.type_mask = 3;
		state.iterator.flags = 0;
		state.iterator.index = 0;
		state.iterator.object_index = NONE;
		while ((state.unit = (s_player_removed_unit *)function_baeb0(&state.iterator)) != 0)
		{
			if (state.unit->player_index == *player_reference)
				state.unit->player_index = NONE;
		}
	}
	function_14cad0(*player_reference, NONE);
	player->previous_unit_index = NONE;
	if (g_4cf770 && !g_4cf77b)
		function_69580((c_class_6a600 *)g_4cf77c, *player_reference);
	player->machine_index = NONE;
	memset(player->machine_address, 0, sizeof(player->machine_address));
	player->machine_user = NONE;
	player->machine_controller = NONE;
	s_tail_player *local_player = tail_player_get(*player_reference);
	if (local_player->local_index != NONE)
	{
		player_control_set_unit(local_player->local_index, NONE);
		function_1682bf(NONE, local_player->local_index, NONE);
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->users[local_player->local_index] = NONE;
		globals->user_count--;
		local_player->local_index = NONE;
	}
	s_tail_player *controller_player = tail_player_get(*player_reference);
	if (controller_player->controller_index != NONE)
	{
		s_player_census_counts *globals = (s_player_census_counts *)g_4e8c20;
		globals->controllers[controller_player->controller_index] = NONE;
		globals->controller_count--;
		controller_player->controller_index = NONE;
	}
	player->flags |= 2;
	player->leave_time = g_510c54->game_time;
	player->flags &= ~1;
	((s_player_census_counts *)g_4e8c20)->count--;
	function_157de0(*player_reference);
}

struct s_player_target_unit
{
	byte unknown00[0x30];
	point3f position;
	real radius;
	byte unknown40[0x10a - 0x40];
	byte flags;
	byte unknown10b[0x248 - 0x10b];
	long parent_index;
	byte unknown24c[0x348 - 0x24c];
	byte flags348;
};

__declspec(noinline) bool function_f5dc0(long object_index);
bool function_cc0c0(long unit_index);
short __stdcall function_c8ef0(long unit_index, long object_index, long *target_index, short *seat_index);

void function_151430(long object_index, long player_index, s_target_candidate *best);

static __forceinline void player_object_target_candidate_evaluate(long object_index, long player_index, s_target_candidate *best)
{
	long const *player_reference = &player_index;
	s_target_candidate *const *best_reference = &best;
	s_tail_player *player = tail_player_get(*player_reference);
	s_player_target_unit *object = (s_player_target_unit *)tail_object_get(object_index);
	if (!(object->flags & 4))
	{
		if (!function_f5dc0(object_index))
		{
			long unit_index = player->unit_index;
			if (!function_cc0c0(unit_index) && !(player->flags & 0x800))
			{
				long target_index = NONE;
				long seat_index = NONE;
				short type = function_c8ef0(unit_index, object_index, &target_index, (short *)&seat_index);
				if (type)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					switch (type)
					{
					case 1: candidate.priority = 4; break;
					case 2: candidate.priority = 5; break;
					case 3: candidate.priority = 6; break;
					}
					candidate.object_index = target_index;
					*(short *)&candidate.flags = (short)seat_index;
					function_151320(&candidate, *best_reference, *player_reference);
				}
			}
		}
		else if (player->unit_index != NONE)
		{
			s_player_target_unit *unit = (s_player_target_unit *)tail_object_get(player->unit_index);
			if (!(object->flags348 & 0x40) && object->parent_index == NONE)
			{
				vector3f delta;
				vector3d_from_points3d(&unit->position, &object->position, &delta);
				real radius = unit->radius + object->radius + 0.2f;
				real distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				if (distance < radius * radius)
				{
					s_target_candidate candidate;
					memset(&candidate, 0, sizeof(candidate));
					candidate.priority = 8;
					candidate.object_index = object_index;
					function_151320(&candidate, *best_reference, *player_reference);
				}
			}
		}
	}
}

// @retail 0x151430
void function_151430(long object_index, long player_index, s_target_candidate *best)
{
	player_object_target_candidate_evaluate(object_index, player_index, best);
}

struct s_tail_transfer_options
{
	byte unknown00[0x2d4];
	s_tail_options_player players[16];
};

struct s_tail_transfer_unit
{
	byte unknown00[0x1fc];
	short seat_index;
	byte unknown1fe[0x212 - 0x1fe];
	char primary_slot;
	char secondary_slot;
	byte unknown214[4];
	long weapons[4];
	long weapon_times[4];
	byte unknown238[6];
	byte grenades[2];
};

struct s_tail_transfer_traits
{
	bool valid;
	byte unknown01;
	s_tail_unit_state weapons[3];
	byte grenades[2];
};

// @retail 0x152660
void function_152660(s_tail_transfer_options *options)
{
	bool alternate = false;
	for (long i = 0; i < 16; i++)
		memcpy(options->players[i].traits, ((s_tail_options *)g_4e6948)->players[i].traits, sizeof(options->players[i].traits));
	s_tail_scenario *scenario = (s_tail_scenario *)g_4e0350;
	if (scenario->entry_count > 0 && scenario->entries[0].value2c == 1)
		alternate = true;
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	s_tail_player *player;
	while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
	{
		s_tail_transfer_traits *traits = 0;
		for (long i = 0; i < 16; i++)
		{
			if (memcmp(((s_tail_player_identifier *)player)->identifier, options->players[i].identifier, 12) == 0)
				traits = (s_tail_transfer_traits *)&options->players[i].traits[alternate ? 1 : 0];
		}
		if (traits)
		{
			long unit_index = player->unit_index;
			if (unit_index != NONE)
			{
				s_tail_transfer_unit *unit = (s_tail_transfer_unit *)tail_object_get(unit_index);
				long primary = NONE;
				long secondary = NONE;
				long other = NONE;
				if (unit->seat_index != NONE)
				{
					long now = g_510c54->game_time;
					for (long selection = 0; selection < 2; selection++)
					{
						long chosen = NONE;
						long shortest = 0x7fffffff;
						for (long slot = 0; slot < 4; slot++)
						{
							long weapon = unit->weapons[slot];
							long age = now - unit->weapon_times[slot];
							if (weapon != NONE && (!selection || weapon != primary) && age < shortest)
							{
								shortest = age;
								chosen = weapon;
							}
						}
						if (!selection)
							primary = chosen;
						else
							other = chosen;
					}
				}
				else
				{
					s_tail_transfer_unit *first = (s_tail_transfer_unit *)tail_object_get(unit_index);
					if (first->primary_slot != NONE)
						primary = first->weapons[first->primary_slot];
					s_tail_transfer_unit *second = (s_tail_transfer_unit *)tail_object_get(unit_index);
					if (second->secondary_slot != NONE)
						secondary = second->weapons[second->secondary_slot];
					other = function_cbe60(unit_index);
				}
				if (primary != NONE)
					function_152580(primary, &traits->weapons[0]);
				else
					traits->weapons[0].value0 = 0;
				if (secondary != NONE)
					function_152580(secondary, &traits->weapons[2]);
				else
					traits->weapons[2].value0 = 0;
				if (other != NONE)
					function_152580(other, &traits->weapons[1]);
				else
					traits->weapons[1].value0 = 0;
				traits->grenades[0] = unit->grenades[0];
				traits->grenades[1] = unit->grenades[1];
				traits->valid = true;
			}
			else
				memset(traits, 0, sizeof(*traits));
		}
	}
}

struct s_player_pickup_unit
{
	byte unknown00[0x14];
	long parent_index;
	byte unknown18[0x212 - 0x18];
	char held_slots[2];
	byte unknown214[2];
	char pending_slots[2];
	long weapons[4];
};

struct s_player_weapon_definition
{
	byte unknown00[0x12c];
	dword flags;
};

static __forceinline dword player_weapon_flags(long weapon_index)
{
	s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
	return ((s_player_weapon_definition *)g_4e3b44[weapon->definition_index & 0xffff].bytes)->flags;
}

// @retail 0x1509e0
void function_1509e0(long player_index, long weapon_index, bool *modes)
{
	long const *weapon_reference = &weapon_index;
	bool *const *modes_reference = &modes;
	s_tail_player *player = tail_player_get(player_index);
	s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
	long held[2];
	long pending[2];
	long count = 0;
	{
		long hand = 0;
		if (hand < 2)
		{
			do
			{
				s_player_pickup_unit *current = (s_player_pickup_unit *)tail_object_get(player->unit_index);
				short slot = current->held_slots[hand];
				held[hand] = slot == NONE ? NONE : current->weapons[slot];
				s_player_pickup_unit *next = (s_player_pickup_unit *)tail_object_get(player->unit_index);
				short next_slot = next->pending_slots[hand];
				pending[hand] = next_slot == NONE ? NONE : next->weapons[next_slot];
				if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 3) & 1))
				{
					(*modes_reference)[hand] = false;
					(*modes_reference)[hand + 2] = false;
				}
				hand++;
			} while (hand < 2);
		}
	}
	if (unit->parent_index != NONE)
	{
		{
			long hand = 0;
			if (hand < 2)
			{
				do
				{
					(*modes_reference)[hand] = false;
					(*modes_reference)[hand + 2] = false;
					hand++;
				} while (hand < 2);
			}
		}
	}
	for (long slot = 0; slot < 4; slot++)
	{
		long held_weapon = unit->weapons[slot];
		if (held_weapon != NONE && !(bool)((player_weapon_flags(held_weapon) >> 4) & 1))
		{
			dword flags = player_weapon_flags(*weapon_reference);
			if (!((bool)((flags >> 22) & 1) || (bool)((flags >> 23) & 1)) || held_weapon != held[1] || held_weapon != pending[1])
				count++;
		}
	}
	if (count >= 2)
	{
		for (long hand = 0; hand < 2; hand++)
		{
			if (hand == 1)
			{
				if (held[1] != NONE && !(bool)((player_weapon_flags(held[1]) >> 4) & 1))
					(*modes_reference)[1] = false;
			}
			else if (!(bool)((player_weapon_flags(*weapon_reference) >> 4) & 1))
			{
				(*modes_reference)[hand] = false;
				if (held[hand] != NONE && (bool)((player_weapon_flags(held[hand]) >> 4) & 1))
					(*modes_reference)[hand + 2] = false;
			}
		}
	}
}

#include "unit_requests.h"

void function_a90e0(long first, long second);
void function_a9030(long index, short slot);
void function_a8f80(long index, short slot);
bool __stdcall function_cd4e0(long unit_index, short hand, bool flag);
struct s_16e150_bits;
void function_16de20(short index, s_16e150_bits const *table, dword *output);

// @retail 0x151c10
bool function_151c10(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (candidate->priority == 8 && function_1518f0(candidate, 2))
	{
		if (g_4e6948->mode == 4)
			function_a90e0(player->unit_index, candidate->object_index);
		else
		{
			s_unit_request request;
			request.type = 0x21;
			*(long *)request.arguments = player->unit_index;
			function_e6900(candidate->object_index, &request);
		}
		result = true;
	}
	return result;
}

// @retail 0x151b80
bool function_151b80(long player_index, bool primary)
{
	long unit_index = tail_player_get(player_index)->unit_index;
	bool result = false;
	if (unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE && unit->weapons[slot] != NONE)
		{
			if (g_4e6948->mode == 4)
			{
				function_a9030(unit_index, hand);
				result = true;
			}
			else
				result = function_e68c0(primary ? 9 : 0x13, unit_index);
		}
	}
	return result;
}

// @retail 0x150820
void function_150820(dword *bits, bool local_only)
{
    s_match_globals *table = g_4e0348;
    s_record_pool_iterator iterator;
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    s_tail_player *player;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        if (local_only && player->local_index == NONE) continue;
        short cluster = *(short *)player->unknown2a;
        if (cluster != NONE)
        {
            dword cluster_bits[16];
            function_16de20(cluster, (s_16e150_bits const *)table, cluster_bits);
            for (long word_index = ((table->list_count + 31) >> 5) - 1; word_index >= 0; --word_index)
                bits[word_index] |= cluster_bits[word_index];
        }
    }
}

// @retail 0x151aa0
bool function_151aa0(long player_index, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if (player->unit_index != NONE)
	{
		long hand = !primary;
		s_player_pickup_unit *unit = (s_player_pickup_unit *)tail_object_get(player->unit_index);
		short slot = unit->held_slots[hand];
		if (slot != NONE)
		{
			long weapon_index = unit->weapons[slot];
			bool modes[4];
			if (weapon_index != NONE && function_cd7b0(player->unit_index, weapon_index, modes) && modes[hand])
			{
				if (g_4e6948->mode == 4)
				{
					function_a8f80(player->unit_index, hand);
					result = true;
				}
				else if (function_cd4e0(player->unit_index, hand, false))
					result = true;
			}
		}
	}
	return result;
}

void function_a8f10(long index, long target, short value);
bool function_138880(void);

// @retail 0x151940
bool function_151940(long player_index, s_target_candidate *candidate, bool primary)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	if ((candidate->priority == 1 || candidate->priority == 7) && player_candidate_has_type(candidate, 4))
	{
		long weapon_index = candidate->object_index;
		s_player_pickup_object *weapon = (s_player_pickup_object *)tail_object_get(weapon_index);
		bool modes[4];
		if (*(long *)((byte *)weapon + 0x154) == NONE && function_cd7b0(player->unit_index, weapon_index, modes))
		{
			short mode;
			if (primary)
			{
				if (modes[0]) mode = 3;
				else if (modes[2]) mode = 4;
				else goto done;
			}
			else
			{
				if (modes[1]) mode = 5;
				else if (modes[3]) mode = 6;
				else goto done;
			}
			if (function_138880())
			{
				function_a8f10(player->unit_index, weapon_index, mode);
				result = true;
			}
			else
			{
				s_unit_request request;
				memset(&request, 0, sizeof(request));
				request.type = 0x14;
				*(long *)request.arguments = weapon_index;
				*(short *)(request.arguments + 4) = mode;
				if (function_e6900(player->unit_index, &request)) result = true;
			}
		}
	}
 done:
	return result;
}

void function_a9180(long first, long second);
void function_a9340(long first, long second, short seat);
bool __stdcall function_c92c0(long unit_index, long vehicle_index, short seat_index, long *blocker_index, bool *grouped);
void function_107840(long device_index, long unit_index);
bool function_1c9500(long unit_index, long actor_index, bool notify);
void function_1e8f60(long player_index);

// @retail 0x151c90
bool function_151c90(long player_index, s_target_candidate *candidate)
{
	s_tail_player *player = tail_player_get(player_index);
	bool result = false;
	switch (candidate->priority)
	{
	case 3:
		if (function_1518f0(candidate, 0x380))
		{
			if (g_4e6948->mode == 4)
				function_a9180(player->unit_index, candidate->object_index);
			else
				function_107840(candidate->object_index, player->unit_index);
			function_1e8f60(player_index);
			result = true;
		}
		break;
	case 4:
	case 5:
	case 6:
		if (function_1518f0(candidate, 3))
		{
			short seat = *(short *)&candidate->flags;
			if (seat >= 0)
			{
				long target = candidate->object_index;
				long definition = *(long *)tail_object_get(target);
				byte *tag = g_4e3b44[definition & 0xffff].bytes;
				if (seat < *(long *)(tag + 0x1c8))
				{
					long blocker = NONE;
					bool grouped;
					if (function_c92c0(player->unit_index, target, seat, &blocker, &grouped))
					{
						if (function_138880())
							function_a9340(player->unit_index, candidate->object_index, seat);
						else
						{
							s_unit_request request;
							request.type = 0x1c;
							request.type1c.object_index = candidate->object_index;
							request.type1c.seat_index = seat;
							request.type1c.unknowna = false;
							request.type1c.unknownb = false;
							function_e6900(player->unit_index, &request);
						}
						result = true;
					}
					else if (blocker != NONE)
					{
						byte *unit = (byte *)tail_object_get(blocker);
						if (!function_138880() && *(long *)(unit + 0x12c) != NONE)
							function_1c9500(player->unit_index, *(long *)(unit + 0x12c), true);
						result = true;
					}
				}
			}
		}
		break;
	case 2:
		if (g_4e6948->mode != 4)
		{
			s_unit_request request;
			request.type = 0x15;
			*(long *)request.arguments = player_index;
			result = function_e6900(candidate->object_index, &request);
		}
		break;
	}
	return result;
}

struct s_player_rebind_option
{
    bool active;
    byte unknown01;
    short local_index;
    long controller_index;
    byte unknown08[6];
    byte identifier[12];
    byte unknown1a[0xe4 - 0x1a];
};
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;
void __stdcall function_167e86(long user_index, long hand);
void __stdcall player_set_local_user(long player_index, long user_index);
void __stdcall function_14f270(long player_index, long controller_index);

// @retail 0x152f80
void __stdcall function_152f80(void *address, void *option_data)
{
    byte const *machine_address = (byte const *)address;
    s_player_rebind_option const *options = (s_player_rebind_option const *)option_data;
    long controllers[16] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
    long users[16] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
    long assigned[16] = { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE };
    dword options_used = 0;
    dword users_used = 0;
    dword controllers_used = 0;
    s_record_pool_iterator iterator;
    s_tail_player *player;
    iterator.data = g_4e8c24;
    for (long pass = 0; pass < 4; pass++)
    {
        iterator.index = NONE;
        while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
        {
            long index = (short)iterator.datum_index;
            if (assigned[index] != NONE) continue;
            for (long option = 0; option < 16; option++)
            {
                s_player_rebind_option const *entry = &options[option];
                if (!entry->active || (options_used & (1 << option))) continue;
                switch (pass)
                {
                case 0:
                    if (player->local_index == NONE || memcmp(entry->identifier, player->unknown04, 12)) continue;
                    break;
                case 1:
                    if (player->local_index == NONE || entry->controller_index != player->controller_index) continue;
                    break;
                case 2:
                    if (player->local_index != NONE) continue;
                    break;
                case 3:
                    break;
                default:
                    __assume(0);
                }
                users[index] = entry->local_index;
                assigned[index] = option;
                controllers[index] = entry->controller_index;
                options_used |= 1 << option;
                users_used |= 1 << entry->local_index;
                controllers_used |= 1 << entry->controller_index;
                break;
            }
        }
    }
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (assigned[index] != NONE || player->local_index == NONE) continue;
        long controller = NONE;
        long user = NONE;
        for (long pass = 0; pass < 3; pass++)
        {
            if (controller != NONE) break;
            for (long candidate = 0; candidate != NONE; candidate = candidate >= 0 && candidate < 3 ? candidate + 1 : NONE)
            {
                if (!(controllers_used & (1 << candidate)) &&
                    (pass > 0 || candidate == player->controller_index) &&
                    (pass > 1 || g_4e61cc[(short)candidate]))
                {
                    controller = candidate;
                    break;
                }
            }
        }
        for (long pass = 0; pass < 2; pass++)
        {
            if (user != NONE) break;
            for (long candidate = 0; candidate < 4; candidate++)
            {
                if (!(users_used & (1 << candidate)) && (pass > 0 || candidate == player->local_index))
                {
                    user = candidate;
                    break;
                }
            }
        }
        controllers[index] = controller;
        users[index] = user;
        users_used |= 1 << user;
        controllers_used |= 1 << controller;
    }
    byte *globals = (byte *)g_4e8c20;
    if (*(long *)(globals + 0x98) != NONE)
    {
        iterator.index = NONE;
        while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
            if (player->local_index != NONE) memcpy(player->machine_address, machine_address, 6);
        memcpy(globals + 0x91, machine_address, 6);
        memcpy(globals + 0x30 + *(long *)(globals + 0x98) * 6, machine_address, 6);
    }
    else
    {
        globals[0x90] = 1;
        memcpy(globals + 0x91, machine_address, 6);
        *(long *)(globals + 0x98) = 0;
        *(dword *)(globals + 0x2c) |= 1;
        memcpy(globals + 0x30, machine_address, 6);
    }
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (player->local_index != NONE && (player->local_index != users[index] || player->controller_index != controllers[index]))
        {
            s_tail_player *local = tail_player_get(iterator.datum_index);
            if (local->local_index != NONE)
            {
                player_control_set_unit(local->local_index, NONE);
                long user = local->local_index;
                byte *state = (byte *)g_4e9bc8 + user * 0x20cc;
                if (*(long *)(state + 4) != NONE)
                {
                    for (long hand = 0; hand < 2; hand++)
                    {
                        dword *flags = (dword *)(state + 0xc + hand * 0x1010);
                        if (*flags & 1) *flags &= ~1;
                    }
                    *(dword *)state &= ~1;
                    *(long *)(state + 4) = NONE;
                    *(long *)(state + 8) = NONE;
                    for (long hand = 0; hand < 2; hand++) function_167e86(user, hand);
                }
                s_player_census_counts *census = (s_player_census_counts *)g_4e8c20;
                census->users[local->local_index] = NONE;
                census->user_count--;
                local->local_index = NONE;
            }
            s_tail_player *local_controller = tail_player_get(iterator.datum_index);
            if (local_controller->controller_index != NONE)
            {
                s_player_census_counts *census = (s_player_census_counts *)g_4e8c20;
                census->controllers[local_controller->controller_index] = NONE;
                census->controller_count--;
                local_controller->controller_index = NONE;
            }
            player->machine_user = NONE;
            player->machine_controller = NONE;
        }
    }
    iterator.data = g_4e8c24;
    iterator.index = NONE;
    while ((player = (s_tail_player *)data_iterator_next_inlined(&iterator)) != 0)
    {
        long index = (short)iterator.datum_index;
        if (users[index] == NONE) continue;
        memcpy(player->machine_address, machine_address, 6);
        player->machine_index = (short)*(long *)((byte *)g_4e8c20 + 0x98);
        if (player->local_index == NONE)
        {
            player->machine_user = (short)users[index];
            player->machine_controller = controllers[index];
            player_set_local_user(iterator.datum_index, player->machine_user);
            s_tail_player *local = tail_player_get(iterator.datum_index);
            long controller = player->machine_controller;
            if (local->controller_index != controller)
            {
                if (controller == NONE)
                {
                    s_player_census_counts *census = (s_player_census_counts *)g_4e8c20;
                    census->controllers[local->controller_index] = NONE;
                    census->controller_count--;
                    local->controller_index = NONE;
                }
                else
                {
                    function_14f270(iterator.datum_index, NONE);
                    local->controller_index = controller;
                    s_player_census_counts *census = (s_player_census_counts *)g_4e8c20;
                    census->controllers[controller] = iterator.datum_index;
                    census->controller_count++;
                }
            }
        }
        s_player_slot *slot = &g_54e8e0[player->controller_index];
        if (TEST_FIELD_BIT(slot->flag4)) *(long *)slot->unknown004 = player->local_index;
    }
}

#include "data_array.h"
void __stdcall function_14be90(long player_index, dword const *configuration);

struct s_player_configuration_copy
{
    dword words[0x24];
};

// @retail 0x1523c0
void function_1523c0()
{
    s_record_pool *players = g_4e8c24;
    long index = NONE;
    for (;;)
    {
        long next = data_find_index(players, index + 1);
        if (next == NONE)
            break;
        byte *player = players->data + players->size * next;
        long datum = ((long)*(short const volatile *)player << 16) | next;
        s_player_configuration_copy configuration;
        index = next;
        memcpy(&configuration, player + 0xd4, sizeof(configuration));
        function_14be90(datum, configuration.words);
    }
}
