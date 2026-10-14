// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1E0DC0.CPP: creating an actor and sending its unit a request
   (built with SSE, apart from its neighbours in
   unknown_1e1f20.cpp) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_2551c0.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_19ec40.h"
#include "unknown_1e46c0.h"
#include <string.h>
#include "unit_requests.h"

extern bool g_4f55e8;
void function_caf00(long unit_index);
long function_1e4a10(long tag_index);
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality);
bool function_1df5d0(short first, short second);
bool function_1e11b0(long actor_index, bool active);
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra);
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag);

// @retail 0x1e0dc0
long function_1e0dc0(short team, long unit_index, short request_value, short squad_index)
{
	const long *unit_reference = &unit_index;
	const short *request_reference = &request_value;
	const short *squad_reference = &squad_index;
	byte *unit = (byte *)ai_object_get(*unit_reference);
	function_caf00(*unit_reference);
	real *limits = (real *)function_1e4a10(*(long *)(unit + 0x3d8));
	if (limits)
	{
		short difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : (short)1;
		real body, shield;
		switch (difficulty)
		{
		case 3:
			body = limits[3];
			shield = limits[4];
			if (body <= 0.0f && shield <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			break;
		case 2:
			if (limits[3] <= 0.0f && limits[4] <= 0.0f)
			{
				body = limits[1];
				shield = limits[2];
			}
			else
			{
				body = (limits[1] + limits[3]) * 0.5f;
				shield = (limits[4] + limits[2]) * 0.5f;
			}
			break;
		default:
			body = limits[1];
			shield = limits[2];
			break;
		}
		if (g_4e6948->state == 1 && g_4f55e8)
		{
			body *= 2.0f;
			shield *= 2.0f;
		}
		if (body > 0.0f || shield > 0.0f)
			function_d5c90(*unit_reference, &body, &shield);
	}
	long definition_index;
	if (*(long *)(unit + 0x3d8) != NONE &&
		(*(short *)(unit + 0x138) == team || function_1df5d0(*(short *)(unit + 0x138), team)))
		definition_index = *(long *)(unit + 0x3d8);
	else
		definition_index = *(long *)(g_4e3b44[*(long *)unit & 0xffff].bytes + 0x304);
	long result;
	if (definition_index != NONE)
	{
		result = function_1e0160(*squad_reference, definition_index, *unit_reference, false);
		if (result != NONE)
		{
			if (*squad_reference == NONE)
				function_1e11b0(result, true);
			s_unit_request request;
			*(short *)(request.arguments + 0) = g_510c54->field_2_3 * 3;
			request.type = 0x31;
			*(short *)(request.arguments + 2) = *request_reference;
			if (function_e6900(*unit_reference, &request))
				function_114b60(NONE, 0x15, actor_get(result)->unknown018, 0xd, NULL);
		}
	}
	else
		result = NONE;
	return result;
}
