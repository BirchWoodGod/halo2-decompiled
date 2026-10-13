// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10AD40.CPP: object helpers of the script functions: an object
   flag that notifies the ai, and the child object attached at a marker */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include <math.h>

struct s_object_10ad40
{
	byte unknown000[0xc];
	long next_object_index;
	long first_child_index;
	byte unknown014[0x18 - 0x14];
	signed char parent_node_index;
	byte unknown019[0x64 - 0x19];
	point3f position;
	byte unknown070[0xc0 - 0x70];
	union
	{
		word flags_c0;
		struct
		{
			word : 5;
			word flag_c0_5 : 1;
		};
		struct /* the same bit, read as a byte */
		{
			byte : 5;
			byte flag_c0_5_byte : 1;
		};
	};
};

struct s_object_header_10ad40
{
	byte unknown00[8];
	s_object_10ad40 *object;
};

#define OBJECT_GET_10ad40(index) (((s_object_header_10ad40 *)g_4e0300->data)[(index) & 0xffff].object)

void __stdcall function_1c3770(long object_index, dword flags);

// @retail 0x10ad40
void function_10ad40(long object_index, bool flag)
{
	if (object_index != NONE)
	{
		s_object_10ad40 *object = OBJECT_GET_10ad40(object_index);
		bool old_flag = TEST_FIELD_BIT(object->flag_c0_5_byte);

		if (flag)
			object->flag_c0_5 = true;
		else
			object->flag_c0_5 = false;
		if (old_flag != flag)
			function_1c3770(object_index, 0);
	}
}

// @retail 0x10ae60
long function_10ae60(long object_index, long marker_name)
{
	if (object_index != NONE)
	{
		long child_index = OBJECT_GET_10ad40(object_index)->first_child_index;
		s_object_marker marker;

		function_b8d30(object_index, marker_name, &marker, 1, false);
		while (child_index != NONE)
		{
			s_object_10ad40 *child = OBJECT_GET_10ad40(child_index);

			if (child->parent_node_index == marker.node_index)
			{
				real dx = child->position.x - marker.node_matrix.position.x;
				real dy = child->position.y - marker.node_matrix.position.y;
				real dz = child->position.z - marker.node_matrix.position.z;

				real length_squared = dx * dx + dz * dz;
				length_squared += dy * dy;
				if (1.0f > sqrt(length_squared))
					return child_index;
			}
			child_index = child->next_object_index;
		}
	}
	return NONE;
}
