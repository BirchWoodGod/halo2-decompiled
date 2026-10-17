// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "unknown_11cc90.h"
#include "unknown_272b70.h"

/* slot type 0x7e: the actors of a group take the positions of a formation
   around their leader */

struct s_slot_7e
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[3];
	long element_index;
	byte unknown14[0x1c - 0x14];
	short position_index;
	short unknown1e;
	long unknown20;
	bool unknown24;
	byte unknown25[3];
	long unknown28;
	long unknown2c;
	byte unknown30[0x40 - 0x30];
};

bool __stdcall function_1f4280(long actor_index);
real normalize2d(point2f *v);

/* the points function_25ab50 validates: g_4e0350 + 0x1dc */
struct s_point_block
{
	byte unknown00[0x24];
	byte *points;
	byte unknown28[0x30 - 0x28];
};

struct s_point_globals
{
	long count;
	s_point_block *blocks;
};

struct s_4e0350_point_view
{
	byte unknown000[0x1dc];
	s_point_globals *points;
};

__forceinline point2f *point_get(long reference)
{
	s_point_block *block = &((s_4e0350_point_view *)g_4e0350)->points->blocks[(reference >> 16) & 0xffff];

	return (point2f *)(block->points + (reference & 0xffff) * 0x3c + 0x20);
}

vector2f *g_468778;

inline real magnitude2d(vector2f const *v)
{
	return (real)sqrt(v->i * v->i + v->j * v->j);
}

inline real normalize2d_fast(vector2f *v)
{
	real magnitude = magnitude2d(v);

	if (fabs(magnitude) < 0.0001f)
		return 0.0f;

	real scale = 1.0f / magnitude;

	v->i *= scale;
	v->j *= scale;
	return magnitude;
}

inline real distance2d(point2f const *a, point2f const *b)
{
	vector2f d;

	d.i = a->x - b->x;
	d.j = a->y - b->y;
	return magnitude2d(&d);
}

short __stdcall function_1b0cd0(long actor_index);
bool __stdcall function_1b13b0(long actor_index, s_slot *slot, long index);

// @retail 0x1b0cd0
short __stdcall function_1b0cd0(long actor_index)
{
	s_slot_entry_iterator iterator;
	long result = 0;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x7e;
	iterator.reference.unknown0 = NONE;
	if (function_26f0c0(&iterator))
		result = 3;
	return result;
}

// @retail 0x1b0d10
short __stdcall function_1b0d10(long actor_index, s_slot *slot, bool active)
{
	s_slot_7e *state = (s_slot_7e *)slot;
	short result = g_46fbe8;

	if (element_502424_get(state->element_index)->unknown04 == NONE)
		result = g_46fbe4;
	return result;
}

extern "C" void _ReadWriteBarrier();
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1b0d50
void __stdcall function_1b0d50(long actor_index, s_slot *slot)
{
	s_slot_7e *state = (s_slot_7e *)slot;
	s_slot_entry_iterator iterator;

	iterator.actor_index = actor_index;
	iterator.reference.unknown2 = 0x7e;
	iterator.reference.unknown0 = NONE;
	for (s_slot_memory_entry *entry = function_26f0c0(&iterator); entry; entry = function_26f0c0(&iterator))
	{
		if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
		{
			long entry_value = entry->unknown4;
			state->position_index = NONE;
			if (entry_value != NONE)
				return;
			break;
		}
	}

	long element_index = function_26e940(actor_index);

	if (element_index != NONE)
	{
		s_slot_target_list *list = (s_slot_target_list *)element_502424_get(element_index);
		bool unknown24 = state->unknown24;
		_ReadWriteBarrier();

		state->unknown0c = true;
		state->element_index = element_index;
		state->position_index = 0;
		if (unknown24)
		{
			list->mode = state->unknown1e;
			list->unknown84 = state->unknown28;
			list->unknown88 = state->unknown2c;
			list->unknown80 = false;
		}
		else
		{
			state->unknown20 = NONE;
			list->mode = 5;
			list->unknown84 = NONE;
			list->unknown80 = false;
		}
	}
}
#pragma function(_ReadWriteBarrier)


PRIVATE __forceinline void function_1b0e41(short arg_0, vector2f const *arg_1,
    vector2f const *arg_2, point2f const *arg_3, point2f *arg_4)
{
	real row = (real)(short)((arg_0 + 1) / 2);
	real distance = row * -0.225f;

	arg_4->x = arg_2->i * distance + arg_3->x;
	arg_4->y = arg_2->j * distance + arg_3->y;

	double side = (arg_0 % 2 - 0.5) * (row * 0.9f);

	arg_4->x += side * arg_1->i;
	arg_4->y += side * arg_1->j;
}

/* the position of a formation slot: in a line behind the leader, or (mode 5)
   in two staggered lines */
// @retail 0x1b0e40
bool function_1b0e40(point2f *point, point2f const *origin, vector2f const *forward, short index,
	short mode, vector2f const *left)
{
	switch (mode)
	{
	case 0:
	{
		real distance = index * 0.45f;

		point->x = forward->i * distance + origin->x;
		point->y = forward->j * distance + origin->y;
		break;
	}
	case 5:
		function_1b0e41(index, left, forward, origin, point);
		break;
	}
	return true;
}

// @retail 0x1b0f20
void function_1b0f20(s_slot_target_list *list, vector2f *direction)
{
	if (function_25ab50(list->unknown84) && function_25ab50(list->unknown88))
	{
		point2f *end = point_get(*(long volatile *)&list->unknown88);
		point2f *start = point_get(*(long volatile *)&list->unknown84);

		direction->i = end->x - start->x;
		direction->j = end->y - start->y;
		if (normalize2d((point2f *)direction) != 0.0f)
			return;
	}
	else
	{
		s_actor_view *leader = actor_get(list->entries[0].actor_index);

		*direction = *(vector2f *)&leader->unknown290;
		if (normalize2d_fast(direction) != 0.0f)
			return;
	}
	*direction = *g_468778;
}

// @retail 0x1b1080
s_slot *function_1b1080(short index, s_slot_target_list *list)
{
	s_slot *result = NULL;

	if (index >= 0 && index < list->entry_count)
	{
		s_slot_target_entry *entry = &list->entries[index];

		if (entry->actor_index != NONE && entry->type == 3)
		{
			s_actor_view *actor = actor_get(entry->actor_index);

			if (actor->current >= 0 && actor->current < 4)
				result = &actor->slots[actor->current];
		}
	}
	return result;
}

// @retail 0x1b10e0
short function_1b10e0(short index, s_slot_target_list *list)
{
	short result = NONE;
	s_slot_7e *state = (s_slot_7e *)function_1b1080(index, list);

	if (state)
		result = state->position_index;
	return result;
}

/* chooses the free formation position nearest the actor */
// @retail 0x1b1100
short function_1b1100(long actor_index, vector2f const *forward, s_slot_7e *state, s_slot_target_list *list,
	point2f const *origin, vector2f const *left, point2f *position)
{
	s_actor_view *actors = (s_actor_view *)g_4f55f0->data;
	long actor_number = actor_index & 0xffff;

	if (state->unknown0c)
		return 0;

	real distances[10];
	point2f points[10];
	short best_index = NONE;
	real best_distance = 3.4028235e38f;
	short count = list->entry_count;

	for (short i = 1; i < count; i++)
	{
		if (function_1b0e40(&points[i], origin, forward, i, list->mode, left))
		{
			real distance = distance2d((point2f *)&actors[actor_number].position, &points[i]);

			distances[i] = distance;
			if (best_distance > distance)
			{
				best_index = i;
				best_distance = distance;
			}
		}
		else
		{
			distances[i] = 3.4028235e38f;
		}
	}
	while (best_index != NONE)
	{
		short other_index = NONE;

		for (short j = 0; j < 10; j++)
		{
			s_slot_7e *other = (s_slot_7e *)function_1b1080(j, list);

			if (other && other->position_index == best_index)
			{
				other_index = j;
				break;
			}
		}
		if (other_index == NONE)
			break;

		s_actor_view *other_actor = &actors[list->entries[other_index].actor_index & 0xffff];

		if (distance2d((point2f *)&other_actor->position, &points[best_index]) + 0.1f > distances[best_index])
		{
			((s_slot_7e *)&other_actor->slots[other_actor->current])->position_index = NONE;
			break;
		}
		distances[best_index] = 3.4028235e38f;
		best_index = NONE;
		best_distance = 3.4028235e38f;
		for (short k = 1; k < count; k++)
		{
			if (best_distance > distances[k])
			{
				best_distance = distances[k];
				best_index = k;
			}
		}
	}
	if (best_index >= 0 && best_index < 10 && position)
		*position = points[best_index];
	state->position_index = best_index;
	return best_index;
}

/* whether the actor stands on the right side of its formation position */
// @retail 0x1b1970
bool function_1b1970(long actor_index, short mode, vector2f const *forward,
	vector2f const *left, short index)
{
	bool result = false;

	switch (mode)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		result = true;
		break;
	case 5:
		if (index == 0)
		{
			result = true;
		}
		else
		{
			s_actor_view *actor = actor_get(actor_index);
			point3f *target = &prop_node_state(prop_node_get(actor->prop_index))->position;
			point2f direction;

			direction.x = target->x - actor->position.x;
			direction.y = target->y - actor->position.y;
			if (normalize2d(&direction) != 0.0f)
			{
				real forward_dot = forward->i * direction.x + forward->j * direction.y;
				real left_dot = left->i * direction.x + left->j * direction.y;

				if (index % 2 == 0)
				{
					if (left_dot <= 0.0f)
					{
						result = true;
						goto done;
					}
				}
				else if (left_dot >= 0.0f)
				{
					result = true;
					goto done;
				}
				if (forward_dot >= 0.9f)
					result = true;
			}
		}
		break;
	}
done:
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1b1a90
void __stdcall function_1b1a90(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	long actor_offset = (actor_index & 0xffff) * sizeof(s_actor_view);
	byte *actor_base = g_4f55f0->data;
	_ReadWriteBarrier();
#define actor (*(s_actor_view *)(actor_base + actor_offset))

	if (list->entries[0].actor_index == NONE)
		return;

	s_slot_7e *state = (s_slot_7e *)slot;
	s_actor_view *leader = actor_get(list->entries[0].actor_index);
	vector2f forward;
	vector2f left;

	forward.i = leader->unknown290.i;
	forward.j = leader->unknown290.j;
	normalize2d_fast(&forward);
	left.i = 0.0f - forward.j;
	left.j = forward.i;
	if (list->unknown80)
	{
		if (!state->unknown0c)
		{
			if (actor.unknown50c)
				function_1f4280(actor_index);
			if (leader->unknown5d0)
			{
				point2f point;

				if (function_1b0e40(&point, (point2f *)&leader->position, &forward, state->position_index, list->mode, &left))
				{
					point2f direction;

					direction.x = point.x - actor.position.x;
					direction.y = point.y - actor.position.y;
					normalize2d(&direction);
					if (leader->unknown290.i * direction.x + leader->unknown290.j * direction.y < 0.5f)
					{
						function_1f4280(actor_index);
					}
					else
					{
						actor.unknown456 = true;
						actor.unknown458.i = direction.x;
						actor.unknown458.j = direction.y;
						actor.unknown458.k = 0.0f;
					}
				}
			}
		}
		if (actor.unknown086 >= 7)
		{
			if (state->position_index != NONE)
				actor.unknown488 = function_1b1970(actor_index, list->mode, &forward, &left, state->position_index);
			else
				actor.unknown488 = false;
			actor.unknown41c = 3;
			actor.unknown420 = 2;
		}
		actor.unknown44a = true;
		actor.unknown449 = true;
	}
	else
	{
		actor.unknown4cc = 0.2f;
		if (actor.unknown504 == 2)
		{
			vector2f direction;

			function_1b0f20(list, &direction);
			actor.unknown420 = 4;
			actor.unknown424.vector.i = direction.i;
			actor.unknown424.vector.j = direction.j;
			actor.unknown44d = true;
			actor.unknown41c = 3;
			actor.unknown424.vector.k = 0.0f;
		}
		else if (actor.unknown086 >= 7)
		{
			actor.unknown488 = false;
			actor.unknown420 = 2;
			actor.unknown41c = 3;
		}
	}
#undef actor
}
#pragma function(_ReadWriteBarrier)

/* invites the actors of the actor's group (or squad) of the same team */
struct s_1b1d90_locals
{
	long field_00;
	byte field_04[0xc];
	s_ai_actor_iterator field_10;
};

// @retail 0x1b1d90
short __stdcall function_1b1d90(long actor_index, long leader_index, s_slot *slot, long unknown)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_7e *state = (s_slot_7e *)slot;
	s_1b1d90_locals locals;
	locals.field_00 = 0;

	if (state->unknown20 != NONE)
	{
		ai_actor_iterator_new(state->unknown20, &locals.field_10);
		for (s_actor_view *other = (s_actor_view *)ai_actor_iterator_next(&locals.field_10); other;
			other = (s_actor_view *)ai_actor_iterator_next(&locals.field_10))
		{
			if (other != actor && actor->unknown004 == other->unknown004 &&
				function_26eae0(leader_index, locals.field_10.actor_index, 3, 1.0f))
			{
				locals.field_00++;
			}
		}
		return (short)locals.field_00;
	}
	if (actor->unknown030 != NONE)
	{
		s_squad_actor_iterator *iterator = (s_squad_actor_iterator *)&locals.field_10;

		function_204d30(iterator, actor->unknown030);
		long other_index = iterator->next_actor_index;

		while (g_4f55d0->active && other_index != NONE)
		{
			s_actor_view *other = actor_get(other_index);
			long index = other_index;

			other_index = other->unknown020;
			if (other != actor && actor->unknown004 == other->unknown004 &&
				function_26eae0(leader_index, index, 3, 1.0f))
			{
				locals.field_00++;
			}
		}
	}
	return (short)locals.field_00;
}

s_slot_handler_2x g_47df60 =
{
	{
		{
			0x7e, 2, 0, -2, 0,
			function_1b0cd0, function_1b0d10, joint_initiate, joint_leave, NONE, {0},
			0, 0, 0, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	function_1b0d50, 0, (t_slot_release)function_1b13b0, 0, function_1b1a90, (t_slot_proc4)function_1b1d90,
	3, 10, 1.0f, 0
};

#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "unknown_1f4460.h"
bool function_1f86f0(long arg_0);
long function_1fa7f0(void);
long __stdcall function_26d0e0(point3f const *arg_0, s_type_c3b527 *arg_1, long arg_2);
void function_210be0(s_type_c3b527 const *arg_0, s_type_c3b527 const *arg_1, vector3f *arg_2);

// @retail 0x1b13b0
bool __stdcall function_1b13b0(long arg_0, s_slot *arg_1, long arg_2)
{
    bool local_3 = true;
    {
        s_actor_view *local_0 = actor_get(arg_0);
        s_slot_7e *local_1 = (s_slot_7e *)arg_1;
        s_slot_target_list *local_2 = (s_slot_target_list *)arg_2;
        long local_4 = local_2->entries[0].actor_index;
        if (local_4 == NONE)
            { local_3 = false; goto done; }
        if (local_2->count != 1)
            goto done;
        if (local_2->unknown80)
        {
            if (!local_1->unknown0c || !local_0->unknown040)
                goto done;
            if (function_25ab50(local_2->unknown88))
            {
                s_type_c3b527 *local_5 = (s_type_c3b527 *)point_get(local_2->unknown88);
                { local_3 = function_1f4460(arg_0, local_5, *(long *)((byte *)local_5 + 0x10), NONE, false); goto done; }
            }
            if (!REFERENCE_EQUAL(local_0->unknown418, g_470fa0) && function_1f86f0(arg_0))
                function_262800(arg_0, local_0->unknown418, false);
            s_2605d0_request local_6 = {0};
            local_6.type = 4;
            local_6.unknown015 = true;
            local_6.unknown05b = true;
            *(real *)((byte *)&local_6 + 0x1c) = 50.0f;
            byte *local_7 = ai_scratch_buffer_get();
            long local_8;
            bool local_9;
            s_reference local_10 = function_2605d0(arg_0, &local_6, 0, (long)&local_8, local_7, &local_9);
            local_10 = function_2626b0(arg_0, local_10, local_8, local_7, local_9, true);
            if (REFERENCE_EQUAL(local_10, g_470fa0))
                local_3 = false;
            ai_scratch_buffer_release(local_7);
            goto done;
        }
        else
        {
            s_actor_view *local_11 = actor_get(local_4);
            s_type_c3b527 local_12;
            vector2f local_13;
            long local_14;
            bool local_15 = false;
            if (local_2->unknown84 != NONE && function_25ab50(local_2->unknown84) &&
                function_25ab50(local_2->unknown88))
            {
                s_type_c3b527 *local_16 = (s_type_c3b527 *)point_get(local_2->unknown84);
                s_type_c3b527 *local_17 = (s_type_c3b527 *)point_get(local_2->unknown88);
                vector3f local_18;
                function_210be0(local_16, local_17, &local_18);
                local_13.i = local_18.i;
                local_13.j = local_18.j;
                if (normalize2d((point2f *)&local_13) == 0.0f)
                    local_13 = *g_468778;
                local_12 = *local_16;
                local_14 = *(long *)((byte *)local_16 + 0x10);
                local_15 = true;
            }
            else
            {
                function_26c180(local_4);
                local_12 = local_11->unknown27c.point;
                local_13.i = local_11->unknown290.i;
                local_13.j = local_11->unknown290.j;
                if (normalize2d((point2f *)&local_13) == 0.0f)
                    local_13 = *g_468778;
                local_14 = NONE;
            }
            point3f local_19;
            function_210850(&local_12, &local_19);
            vector2f local_20;
            local_20.i = 0.0f - local_13.j;
            local_20.j = local_13.i;
            if (local_1->unknown0c)
            {
                bool local_21 = true;
                if (local_15)
                {
                    real local_22 = local_11->position.x - local_19.x;
                    real local_23 = local_11->position.y - local_19.y;
                    if (local_23 * local_23 + local_22 * local_22 > 0.2f ||
                        1.0 - ((double)local_13.j * local_11->unknown290.j +
                            (double)local_13.i * local_11->unknown290.i) > 0.05000000074505806)
                    {
                        local_21 = false;
                        if (local_0->unknown040 && !function_1f4460(arg_0, &local_12, local_14, NONE, false))
                            local_3 = false;
                        local_2->unknown80 = local_21;
                        goto done;
                    }
                }
                for (short local_24 = 0; local_24 < 10; ++local_24)
                {
                    s_slot_target_entry *local_25 = &local_2->entries[local_24];
                    if (local_25->type == 3 && local_25->actor_index != arg_0)
                    {
                        short local_26 = function_1b10e0(local_24, local_2);
                        if (local_26 == NONE)
                        {
                            local_21 = false;
                            break;
                        }
                        s_actor_view *local_27 = actor_get(local_25->actor_index);
                        point2f local_28;
                        if (function_1b0e40(&local_28, (point2f *)&local_19, &local_13,
                            local_26, local_2->mode, &local_20))
                        {
                            real local_29 = local_27->position.x - local_28.x;
                            real local_30 = local_27->position.y - local_28.y;
                            if (local_30 * local_30 + local_29 * local_29 > 0.2f ||
                                1.0 - ((double)local_13.j * local_27->unknown290.j +
                                    (double)local_13.i * local_27->unknown290.i) > 0.05000000074505806)
                            {
                                local_21 = false;
                                break;
                            }
                        }
                    }
                }
                local_2->unknown80 = local_21;
                goto done;
            }
            if (!local_0->unknown040)
                goto done;
            point2f local_31;
            if (local_1->position_index == NONE)
            {
                if (function_1b1100(arg_0, &local_13, local_1, local_2, (point2f *)&local_19,
                    &local_20, &local_31) == NONE)
                    { local_3 = false; goto done; }
            }
            else if (!function_1b0e40(&local_31, (point2f *)&local_19, &local_13,
                local_1->position_index, local_2->mode, &local_20))
                { local_3 = false; goto done; }
            local_19.x = local_31.x;
            local_19.y = local_31.y;
            local_19.z = local_0->position.z;
            long local_33 = function_26d0e0(&local_19, &local_12, function_1fa7f0());
            if (local_33 == NONE)
                { local_3 = false; goto done; }
            { local_3 = function_1f4460(arg_0, &local_12, local_33, NONE, false); goto done; }
        }
    }
done:
    return local_3;
}
