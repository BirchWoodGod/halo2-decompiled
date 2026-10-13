// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_1e3920.h"
#include "unknown_2551c0.h"
#include "object_markers.h"

/* the actor's props as targets */

void *function_1e5380(long actor_index);
void *function_1e5280(long actor_index, long key);
bool function_25d970(s_prop_datum *datum);

bool weapon_barrel_aim(long weapon_index, short barrel_index, point3f const *origin, point3f const *target,
	real const *unknown3, bool unknown5, vector3f *direction, real *time, real *speed_out, real *distance, bool *linear);

struct s_weapon_aim_ranges
{
	byte unknown00[0x38];
	real minimum_distance;
	real maximum_distance;
	real minimum_fraction;
	real maximum_fraction;
};

// @retail 0x1ffbd0
bool function_1ffbd0(long actor_index, long weapon_index, short barrel_index,
	point3f const *origin, point3f const *target, bool flag, vector3f *direction,
	void *unknown0, void *unknown1, void *unknown2, void *unknown3)
{
	long definition_index = moving_object_get(weapon_index)->tag_index;
	s_weapon_aim_ranges *ranges = (s_weapon_aim_ranges *)function_1e5280(actor_index, definition_index);
	bool result = false;
	if (ranges)
	{
		byte *definition = g_4e3b44[definition_index & 0xffff].bytes;
		real fraction = 0.0f;
		bool use_fraction = false;
		if (barrel_index >= 0 && barrel_index < *(long *)(definition + 0x2d0))
		{
			byte *barrel = *(byte **)(definition + 0x2d4) + barrel_index * 0xec;
			s_actor_view *actor = actor_get(actor_index);
			long projectile_index = *(long *)(barrel + 0x90);
			byte *projectile;
			if (projectile_index != NONE &&
				((*(long *)((projectile = g_4e3b44[projectile_index & 0xffff].bytes) + 0xbc) >> 1) & 1) &&
				*(real *)(projectile + 0x164) > 0.0f)
			{
				real distance = distance3d(origin, target);
				real upper = 1.0f - (0.0f > ranges->maximum_fraction ? 0.0f : ranges->maximum_fraction > 1.0f ? 1.0f : ranges->maximum_fraction);
				real lower = 1.0f - (0.0f > ranges->minimum_fraction ? 0.0f : ranges->minimum_fraction > 1.0f ? 1.0f : ranges->minimum_fraction);
				if (distance >= ranges->maximum_distance)
					fraction = upper;
				else if (ranges->minimum_distance >= distance)
					fraction = lower;
				else
					fraction = (distance - ranges->minimum_distance) * (upper - lower) /
						(ranges->maximum_distance - ranges->minimum_distance) + lower;
				use_fraction = true;
			}
			else if (actor->unknown26c == NONE)
			{
				point3f adjusted_origin = actor->position;
				adjusted_origin.z = origin->z;
				return weapon_barrel_aim(weapon_index, barrel_index, &adjusted_origin, target, NULL, flag,
					direction, (real *)unknown1, (real *)unknown0, (real *)unknown2, (bool *)unknown3);
			}
			result = weapon_barrel_aim(weapon_index, barrel_index, origin, target, use_fraction ? &fraction : NULL, flag,
				direction, (real *)unknown1, (real *)unknown0, (real *)unknown2, (bool *)unknown3);
		}
	}
	return result;
}

real function_fa170(long definition_index);
bool function_fa6a0(long definition_index, real const *speed_override, point3f const *origin, point3f const *target,
	real *unknown2, real const *unknown3, real const *unknown4, bool unknown5, vector3f *direction, real *speed_out,
	real *time, real *distance, bool *linear);

struct s_projectile_choice
{
	byte unknown00[0x28];
	long definition_index;
};

struct s_projectile_choices
{
	byte unknown00[0x100];
	long count;
	s_projectile_choice *choices;
};

PRIVATE inline s_projectile_choice *projectile_choice_get(short type)
{
	s_projectile_choices *globals = (s_projectile_choices *)g_4e034c;
	s_projectile_choice *result;
	if (globals->count)
		result = &globals->choices[type];
	else
		result = NULL;
	return result;
}

// @retail 0x1ff200
bool function_1ff200(short type, point3f const *origin, real speed, point3f const *target,
	real const *limit, bool alternate, vector3f *direction, real *speed_out, real *time,
	vector3f *velocity, real *gravity)
{
	real *const *speed_reference = &speed_out;
	s_projectile_choice *choice = projectile_choice_get(type);
	bool result = false;
	if (choice && choice->definition_index != NONE)
	{
		long definition_index = choice->definition_index;
		real adjusted_speed;
		bool linear = false;
		if (function_fa6a0(definition_index, &speed, origin, target, &adjusted_speed, limit, NULL,
			alternate, direction, *speed_reference, time, NULL, &linear) ||
			(adjusted_speed > 0.0f && speed > adjusted_speed &&
			function_fa6a0(definition_index, &(adjusted_speed += 0.01f), origin, target, NULL, limit, NULL,
			alternate, direction, *speed_reference, time, NULL, &linear)))
		{
			result = true;
			if (velocity)
			{
				real scale = **speed_reference;
				velocity->i = direction->i * scale;
				velocity->j = direction->j * scale;
				velocity->k = direction->k * scale;
			}
			if (gravity)
				*gravity = linear ? 0.0f : function_fa170(definition_index);
		}
	}
	return result;
}

/* the block function_1e5380 returns */
struct s_character_variant_ranges
{
	byte unknown00[0x18];
	real minimum_distance;
	real maximum_distance;
};

/* the prop's state as these functions read it */
struct s_prop_state_1ff
{
	byte unknown00[0x3c];
	long unknown3c;
	byte unknown40[0x5e - 0x40];
	bool unknown5e;
};

/* the elements of g_50241c (0xc4 bytes) */
struct s_50241c_element_1ff
{
	byte unknown00[0x23];
	bool unknown23;
	byte unknown24[0xc4 - 0x24];
};

/* whether the point is in the range the actor's variant fights at, and the
   prop's target there */
// @retail 0x1ff6c0
bool function_1ff6c0(long actor_index, long prop_index, point3f const *point, long *target)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	s_character_variant_ranges *ranges = (s_character_variant_ranges *)function_1e5380(actor_index);

	if (ranges)
	{
		real distance = distance3d(&actor->position, point);

		if (distance > ranges->minimum_distance && ranges->maximum_distance > distance)
		{
			if (prop_index != NONE)
			{
				s_prop_node_view *node = prop_node_get(prop_index);
				s_prop_state_1ff *state = (s_prop_state_1ff *)function_25d690((s_prop_datum *)node);
				s_50241c_element_1ff *element = (s_50241c_element_1ff *)(g_50241c->data + (node->unknown08 & 0xffff) * sizeof(s_50241c_element_1ff));

				if (!element->unknown23 || state->unknown5e)
					return result;
				if ((node->unknown24 < 1 || node->unknown24 > 2) && !function_25d970((s_prop_datum *)node))
					return result;
				*target = state->unknown3c;
			}
			return true;
		}
	}
	return result;
}


/* Shared request layout used by the point/radius actor iterator. */
struct s_actor_point_request
{
	byte field_0[0x14];
	s_record_pool *pool;
	long iterator_index;
	long datum_index;
	long group_index;
	long actor_index;
	long next_actor_index;
	short type;
	short mode;
	real radius;
	real radius_squared;
	long index;
	point3f point;
	long result_index;
	real squared_distance;
};


void function_1e4770(s_actor_point_request *request, short mode, point3f const *point, short type, real radius);
s_actor_moving *function_1e47d0(s_actor_point_request *request);

// @retail 0x1ff7d0
bool function_1ff7d0(long actor_index, point3f const *point, real enemy_radius, real friendly_radius, short *count_out)
{
	s_actor_view *actor = actor_get(actor_index);
	real radius = enemy_radius > friendly_radius ? enemy_radius : friendly_radius;
	real enemy_squared = enemy_radius * enemy_radius;
	real friendly_squared = friendly_radius * friendly_radius;
	bool result = true;
	short count = 0;
	s_actor_point_request request;
#pragma inline_depth(0)
	function_1e4770(&request, actor->unknown024, point, 2, radius);
#pragma inline_depth()
	s_actor_view *other = (s_actor_view *)function_1e47d0(&request);
	while (other)
	{
		if (actor->unknown024 != other->unknown024 && function_1df560(actor->unknown024, other->unknown024))
		{
			if (enemy_squared > request.squared_distance)
			{
				long perception_index = *(long *)other->unknown01c;
				if (perception_index != NONE)
					count += *(short *)((byte *)perception_get(perception_index) + 0x14);
				else
					count++;
			}
		}
		else if (friendly_squared > request.squared_distance)
			goto blocked;
		other = (s_actor_view *)function_1e47d0(&request);
	}
	{
		long index = NONE;
		for (;;)
		{
			index = data_next_absolute_index_inlined(g_4e8c24, index + 1);
			if (index == NONE)
				break;
			byte *player = g_4e8c24->data + g_4e8c24->size * index;
			if (!player)
				break;
			long object_index = *(long *)(player + 0x2c);
			if (object_index != NONE)
			{
				s_moving_object *object = moving_object_get(object_index);
				vector3f delta;
				delta.i = ((point3f *)((byte *)object + 0x30))->x - point->x;
				delta.j = ((point3f *)((byte *)object + 0x30))->y - point->y;
				delta.k = ((point3f *)((byte *)object + 0x30))->z - point->z;
				real squared_distance = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
				if (actor->unknown024 != 1 && team_is_enemy(actor->unknown024, 1))
				{
					if (enemy_squared > squared_distance)
						count++;
				}
				else if (friendly_squared > squared_distance)
					goto blocked;
			}
		}
	}
	goto finished;
blocked:
	result = false;
finished:
	if (count_out)
		*count_out = count;
	return result;
}

long function_1e1f20(long actor_index);
point3f *function_b9dd0(long object_index, point3f *result);
void function_1fc710(long actor_index, point3f *position);
real function_30bf0(vector3f *v);
real function_11ce20(vector3f const *a, vector3f const *b);

// @retail 0x1ffe00
bool __stdcall function_1ffe00(long actor_index, long object_index)
{
    s_actor_view *actor = actor_get(actor_index);
    s_slot_object_view *object = object_get(object_index);
    byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
    bool result = false;
    byte *weapon_settings = NULL;
    long weapon_index = function_1e1f20(actor_index);
    if (weapon_index != NONE)
        weapon_settings = (byte *)function_1e5280(actor_index, object_get(weapon_index)->tag_index);
    real best_score = 0.0f;
    long best = NONE;
    if (object->type != 0 || object->player_index == NONE)
    {
        point3f center;
        function_b9dd0(object_index, &center);
        if (weapon_settings)
        {
            real z = center.z - actor->position.z;
            real x = center.x - actor->position.x;
            real y = center.y - actor->position.y;
            real range = *(real *)(weapon_settings + 0x20);
            if (z * z + x * x + y * y < range * range)
            {
                long model_index = *(long *)(definition + 0x38);
                if (model_index != NONE)
                {
                    byte *model = g_4e3b44[model_index & 0xffff].bytes;
                    if (*(long *)(model + 0x68) > 0)
                    {
                        point3f origin;
                        function_1fc710(actor_index, &origin);
                        for (short i = 0; i < *(long *)(model + 0x68); i++)
                        {
                            byte *entry = *(byte **)(model + 0x6c) + i * 0x1c;
                            long name = *(long *)entry;
                            short region = *(short *)(entry + 0xc);
                            if (region >= 0)
                            {
                                byte *current = (byte *)object_get(object_index);
                                if (region >= (*(short *)(current + 0x120) >> 3))
                                    continue;
                                byte *regions = current + *(short *)(current + 0x122);
                                if (regions[region * 8 + 2] == 0xff)
                                    continue;
                            }
                            s_object_marker marker;
                            if (function_b8d30(object_index, name, &marker, 1, false))
                            {
                                vector3f direction;
                                direction.i = origin.x - marker.matrix.position.x;
                                direction.j = origin.y - marker.matrix.position.y;
                                direction.k = origin.z - marker.matrix.position.z;
                                if (function_30bf0(&direction) != 0.0f)
                                {
                                    real angle = function_11ce20(&direction, &marker.matrix.forward);
                                    if (angle < 1.5707963705062866f && angle < *(real *)(entry + 8))
                                    {
                                        real score = 1.0f - angle * 0.6366197466850281f;
                                        if (name == *(long *)((byte *)actor + 0x33c))
                                            score += 0.3f;
                                        if (score > best_score)
                                        {
                                            best_score = score;
                                            best = name;
                                        }
                                    }
                                }
                            }
                        }
                        if (best != NONE)
                        {
                            *(long *)((byte *)actor + 0x33c) = best;
                            return true;
                        }
                    }
                }
            }
        }
    }
    *(long *)((byte *)actor + 0x33c) = 0;
    return result;
}

bool function_1c9290(long arg_0, point3f const *arg_1, vector3f const *arg_2,
    real arg_3, real arg_4, long arg_5, bool arg_6);
real normalize2d(point2f *arg_0);

// @retail 0x1ff010
bool __stdcall function_1ff010(long arg_0, point3f const *arg_1)
{
    bool local_0 = false;
    long const *local_1 = &arg_0;
    point3f const *const *local_2 = &arg_1;
    s_actor_view *local_3 = actor_get(*local_1);
    byte *local_4 = (byte *)function_1e5380(*local_1);
    if (local_4)
    {
        point3f local_5;
        function_210850((s_type_c3b527 const *)((byte *)local_3 + 0x7d0), &local_5);
        s_projectile_choice *local_6 = projectile_choice_get(*(short *)(local_4 + 4));
        long local_7 = NONE;
        if (local_6 && local_6->definition_index != NONE)
            local_7 = local_6->definition_index;
        vector3f local_8;
        real local_9;
        real local_10;
        bool local_11;
        if (function_fa6a0(local_7, NULL, *local_2, &local_5, NULL, NULL,
            (real const *)((byte *)local_3 + 0x7f4), *(bool *)((byte *)local_3 + 0x7c6),
            &local_8, &local_9, &local_10, NULL, &local_11))
        {
            point2f local_13;
            local_13.x = local_8.i;
            local_13.y = local_8.j;
            if (!(normalize2d(&local_13) > 0.0f &&
                local_3->unknown290.j * local_13.y + local_3->unknown290.i * local_13.x > 0.8660253882408142f))
                return false;
            vector3f local_14;
            local_14.i = local_8.i * local_9;
            local_14.j = local_8.j * local_9;
            local_14.k = local_8.k * local_9;
            real local_12;
            if (local_11)
                local_12 = 0.0f;
            else
                local_12 = function_fa170(local_7);
            if (function_1c9290(*local_1, *local_2, &local_14, local_10, local_12,
                *(long *)((byte *)local_3 + 0x7e4), local_3->unknown26c != NONE))
            {
                *(vector3f *)((byte *)local_3 + 0x7e8) = local_8;
                *(real *)((byte *)local_3 + 0x7f4) = local_9;
                local_0 = true;
            }
        }
    }
    return local_0;
}
