// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "data_array.h"
#include "unknown_1fb7e0.h"
#include "unknown_1f4460.h"

/* slot type 0x64's last update: the actor walks to the slot's point and,
   once there, faces its prop */

struct s_slot_64_view
{
	s_slot_header header;
	long prop_index;
	byte unknown10[6];
	bool unknown16;
	bool unknown17;
	bool unknown18;
	byte unknown19[3];
	s_type_c3b527 point;
	byte unknown2c[4];
	real radius;
	byte unknown34[0x40 - 0x34];
};

bool __stdcall function_1f4280(long actor_index);

/* the squared distance from b to a */
static inline real distance_squared3d_1b3c60(point3f const *a, point3f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real dz = a->z - b->z;

	return dz * dz + dy * dy + dx * dx;
}

void __stdcall function_1b3c60(long actor_index, s_slot *slot);

// @retail 0x1b3c60
void __stdcall function_1b3c60(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_64_view *state = (s_slot_64_view *)slot;
	s_type_c3b527 *destination = &state->point;
	real distance_squared;

	if (destination->output_index == NONE)
	{
		vector3f v;

		v.i = actor->position.x - destination->point.x;
		v.j = actor->position.y - destination->point.y;
		v.k = actor->position.z - destination->point.z;

		real sum = v.i * v.i;
		sum += v.k * v.k;
		sum += v.j * v.j;
		distance_squared = sum;
	}
	else
	{
		point3f point;

		function_210850(destination, &point);
		distance_squared = distance_squared3d_1b3c60(&actor->position, &point);
	}
	if (state->radius * state->radius > distance_squared)
		function_1f4280(actor_index);
	if (state->prop_index != NONE)
	{
		if (distance_squared < 4.0f)
		{
			if (!state->unknown16)
			{
				function_1fb7e0(0x37, actor_index, NULL, prop_node_get(state->prop_index)->object_index, NONE);
				state->unknown16 = true;
			}
			state->unknown18 = true;
			actor->unknown41c = 3;
			actor->unknown420 = 1;
			actor->unknown424.object_index = state->prop_index;
			actor->unknown430 = 2;
			actor->unknown434 = 1;
			actor->unknown438.object_index = state->prop_index;
			actor->unknown444 = NONE;
			actor->unknown449 = true;
			actor->unknown44a = false;
			actor->unknown488 = false;
		}
		else
		{
			actor->unknown41c = 2;
			actor->unknown420 = 2;
			actor->unknown488 = false;
		}
	}
}
