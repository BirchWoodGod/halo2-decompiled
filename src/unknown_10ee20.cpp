// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10EE20.CPP: queries of a unit's animation state (the state at the
   unit's offset +0x12a, unknown_1cafc0.cpp's s_animation_state). The rest of
   this file's functions are in unknown_10db60.cpp and unknown_10dc70.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"

/* the unit (a view of the object data) */
struct s_unit_animation_view
{
	long definition_index;
	byte unknown004[0xb3 - 4];
	byte value_b3;
	byte unknown0b4[0x12a - 0xb4];
	short animation_state_offset;
	byte unknown12c[0x33e - 0x12c];
	short control_offset;
};

struct s_unit_animation_header
{
	byte unknown00[8];
	s_unit_animation_view *unit;
};

#define GRAPH_GET(index) ((s_graph_tag *)g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x10f340
bool function_10f340(long unit_index, long mode, long set)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, 0x7000101, 0x7000101, set, 0, &animation_id);
	if (animation_id.index != NONE)
		return true;
	return false;
}

// @retail 0x10f3b0
byte function_10f3b0(long unit_index, long mode, long set)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);

	if (mode == 0x7000101)
	{
		mode = state->unknown70;
		if (mode == NONE)
			mode = 0x6000086;
	}
	else if (mode == 0x7000001)
	{
		mode = 0x6000086;
	}
	c_type_709360 animation_id = GRAPH_GET(state->graph_tag_index)->overlay_get(mode, state->unknown74, state->unknown78,
		set, NULL, NULL, NULL);
	return animation_id.index != NONE;
}

// @retail 0x10fcd0
byte function_10fcd0(long unit_index, long mode, long weapon_class, long weapon_type)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, weapon_class, weapon_type, 0x400000c, 2, &animation_id);
	return animation_id.index != NONE;
}

/* a tag's definition: the unit's (its model at +0x38) and the model's (its
   render model at +4) */
struct s_unit_animation_definition
{
	byte unknown00[0x38];
	long model_tag_index;
};

struct s_unit_animation_model_definition
{
	byte unknown00[4];
	long render_model_tag_index;
};

#define TAG_GET(index) (g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x10f9b0
bool function_10f9b0(long unit_index, long mode, long set, long lookup_flags, transform4x3f *matrix, bool any_weapon)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	bool result = false;
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, any_weapon ? 0x7000001 : 0x7000101, 0x7000001, set, lookup_flags,
		&animation_id);
	if (animation_id.index != NONE)
	{
		s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_GET(unit->definition_index);
		s_unit_animation_model_definition *model = (s_unit_animation_model_definition *)TAG_GET(definition->model_tag_index);

		state->animation_matrix_get(animation_id, 0.0f, (long)TAG_GET(model->render_model_tag_index), matrix);
		result = true;
	}
	return result;
}

bool function_0e6800(c_animation_channel const *channel);

PRIVATE __forceinline bool channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

/* whether a channel has stopped playing (0xe6800, inlined) */
PRIVATE inline long channel_stopped(c_animation_channel const *channel)
{
	long playing = (channel->flags & 1) && !(channel->unknown11 & 9);

	return !(byte)playing;
}

// @retail 0x10ee20
bool function_10ee20(s_animation_state *state)
{
	bool result = (state->flags & 1) != 0;

	if (!result)
	{
		if (channel_valid(&state->channels[2]))
			result = channel_stopped(&state->channels[2]);
		else if (channel_valid(&state->channels[0]))
			result = function_0e6800(&state->channels[0]);
	}
	return result;
}

void function_1dacb0(s_graph_tag *graph, c_type_709360 animation_id, real *event_distance, real *distance);

bool function_10fa80(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance);

// @retail 0x10fa80
bool function_10fa80(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance)
{
	volatile bool result = false;
	if (unit_index == NONE || ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit->value_b3 <= 0)
	{
		s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
		s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
		c_type_709360 found_id;
		s_animation_names names;
		s_animation_names found;
		s_animation *animation;
		state->animation_lookup(&names, &found, mode, 0x7000101, 0x7000101, set, 1, &found_id);
		c_type_709360 animation_id = found_id;
		if (animation_id.index != NONE)
			animation = function_1daea0(GRAPH_GET(state->graph_tag_index), animation_id);
		else
		{
			byte channel_storage[sizeof(c_animation_channel)];
			c_animation_channel &channel = *(c_animation_channel *)channel_storage;
			channel.copy_from((c_animation_channel *)((byte *)unit + unit->control_offset + 0x9c));
			animation = NULL;
			if (state->overlay_play(&channel, 0x3f, set, 0x7000101, 0x7000101) && channel_valid(&channel))
			{
				animation_id = channel.animation_id;
				animation = channel.function_1c6440();
			}
		}
		if (animation)
		{
			long event_frame = function_1dae20(animation);
			long frames = animation->frame_count;
			function_1dacb0(GRAPH_GET(state->graph_tag_index), animation_id, distance, event_distance);
			if (event_frame != NONE)
			{
				real count = (real)(event_frame * (1.0f / 30.0f)) * g_510c54->field_2_3;
				long ticks;
				__asm
				{
					fld count
					fistp ticks
				}
				*event_ticks = (short)ticks;
			}
			else
				*event_ticks = NONE;
			real count = (real)(frames * (1.0f / 30.0f)) * g_510c54->field_2_3;
			long ticks;
			__asm
			{
				fld count
				fistp ticks
			}
			*duration_ticks = (short)ticks;
			result = true;
		}
	}
	return result;
}
