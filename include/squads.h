/* SQUADS.H: the squads, the squad groups and the actors as their iterators
   see them (src/squads.cpp), and the iterators themselves */

#ifndef SQUADS_H
#define SQUADS_H

#include "unknown_11c920.h"
#include "globals.h"

/* the squad groups (g_51e9dc), 0x38 bytes each: a tree of groups, each with
   a list of squads */
struct s_squad_group_datum
{
	byte unknown00[4];
	long first_child_index;
	long first_squad_index;
	long next_sibling_index;
	long parent_index;
	byte unknown14[0x2c - 0x14];
	short value2c;
	byte unknown2e[0x38 - 0x2e];
};

/* the squads (g_51e9d8), 0x98 bytes each */
struct s_squad_datum
{
	byte unknown00[2];
	word flag0 : 1;
	word flag1 : 1;
	word : 7;
	word flag9 : 1;
	word : 6;
	byte unknown04[4];
	short actor_count;
	short count_a;
	short count_c;
	byte unknown0e[2];
	real value10;
	byte unknown14[2];
	short value16;
	byte unknown18[0x24 - 0x18];
	short value24;
	byte unknown26[0x68 - 0x26];
	long first_actor_index;
	byte unknown6c[0x70 - 0x6c];
	long first_vehicle_index;
	short next_squad_index;
	byte unknown76[0x98 - 0x76];
};

/* the actors (g_4f55f0), 0x888 bytes each; slot_handler.h's s_actor_view
   (actor_get) is another view of the same array */
struct s_actor_datum
{
	byte unknown000[0xa];
	bool flag00a;
	bool flag00b;
	bool flag00c;
	byte unknown00d[0x18 - 0xd];
	long unit_index;
	long perception_index;
	long next_actor_index;
	byte unknown024[0x30 - 0x24];
	long squad_index;
	byte unknown034[0x38 - 0x34];
	long starting_location_name;
	byte unknown03c[0x7c - 0x3c];
	long clump_object_index;
	byte unknown080[0x86 - 0x80];
	short value086;
	byte unknown088[0x223 - 0x88];
	bool flag223;
	byte unknown224[0x228 - 0x224];
	bool flag228;
	byte unknown229[0x238 - 0x229];
	point3f position;
	byte unknown244[0x26c - 0x244];
	long unknown26c;
	byte unknown270[0x620 - 0x270];
	short value620;
	byte unknown622[0x858 - 0x622];
	long command_script_index;
	long active_command_script_index;
	byte unknown860[0x888 - 0x860];
};

extern s_record_pool *g_51e9dc;

inline s_squad_group_datum *squad_group_get(long squad_group_index)
{
	return (s_squad_group_datum *)g_51e9dc->data + (squad_group_index & 0xffff);
}

inline s_squad_datum *squad_get(long squad_index)
{
	return (s_squad_datum *)g_51e9d8->data + (squad_index & 0xffff);
}

inline s_actor_datum *actor_datum_get(long actor_index)
{
	return (s_actor_datum *)g_4f55f0->data + (actor_index & 0xffff);
}

/* walks the actors of a squad (or all actors, for NONE) */
struct s_squad_actor_iterator
{
	long squad_index;
	long actor_index;
	long next_actor_index;
};

/* walks the squads of a squad group and of all the groups below it */
struct s_squad_group_iterator
{
	s_squad_group_datum *group;
	s_squad_group_datum *root;
	long squad_index;
	long next_squad_index;
	long previous_squad_index;
};

/* the walk's first step and next step (0x204d30, 0x204d70), which retail
   also inlines into callers such as 0x1c7bb0 */
inline void squad_actors_begin(s_squad_actor_iterator *iterator, long squad_index)
{
	if (g_4f55d0->active)
	{
		iterator->squad_index = squad_index;
		iterator->actor_index = NONE;
		if (squad_index == NONE)
			iterator->next_actor_index = g_4f55d0->unknown14;
		else
			iterator->next_actor_index = squad_get(squad_index)->first_actor_index;
	}
}

inline s_actor_datum *squad_actors_next(s_squad_actor_iterator *iterator)
{
	s_actor_datum *actor = NULL;
	if (g_4f55d0->active && iterator->next_actor_index != NONE)
	{
		actor = actor_datum_get(iterator->next_actor_index);
		iterator->actor_index = iterator->next_actor_index;
		iterator->next_actor_index = actor->next_actor_index;
	}
	return actor;
}

void function_204d30(s_squad_actor_iterator *iterator, long squad_index);
s_actor_datum *function_204d70(s_squad_actor_iterator *iterator);
void function_204db0(s_squad_group_iterator *iterator, long squad_group_index);
s_squad_datum *function_204e10(s_squad_group_iterator *iterator);

#endif
