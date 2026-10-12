// @flags /O2 /Gr
/* UNKNOWN_108FD0.CPP: per-object-type event dispatch (continued from
   unknown_108a90.cpp), and the object list state that follows it */

#include "unknown_11c920.h"
#include "globals.h"
#include "crc.h"
#include "unknown_123b30.h"
#include "object_list.h"
#include <string.h>

struct s_object_handlers
{
	byte unknown00[0x64];
	bool (__stdcall *handler64)(long);
	void (__stdcall *handler68)(long, long, long, long);
	void (__stdcall *handler6c)(long, long, long, long);
	void (__stdcall *handler70)(long, long);
	void (__stdcall *handler74)(long, long, long);
	void (__stdcall *handler78)(long);
	void (__stdcall *handler7c)(long, long, long);
};

struct s_object_type_definition
{
	byte unknown00[0x84];
	s_object_handlers *handlers[1];
};

struct s_object
{
	dword tag_index;
	byte unknown04[0xa6];
	signed char type;
	byte unknownab[0x15];
	byte flags_c0;
};

struct s_object_header
{
	byte unknown00[8];
	s_object *object;
};

struct s_tag_flags
{
	byte unknown00[2];
	unsigned short flag0 : 1;
	unsigned short flag1 : 1;
	unsigned short flag2 : 1;
	unsigned short flag3 : 1;
	unsigned short flag4 : 1;
	unsigned short flag5 : 1;
	unsigned short flag6 : 1;
};

s_object_list_state *g_5107f0;

#define OBJECT_TYPE_DEFINITION(index) (g_468630[((s_object_header *)g_4e0300->data)[(index) & 0xFFFF].object->type])

// @retail 0x108fd0
bool function_108fd0(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	bool result = false;
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler64)
		{
			if (handlers->handler64(object_index))
			{
				result = true;
			}
		}
		i++;
		slot = &definition->handlers[i];
	}
	return result;
}

// @retail 0x109050
void function_109050(long object_index, long a, long b, long c)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler6c)
		{
			handlers->handler6c(object_index, a, b, c);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x1090d0
void function_1090d0(long object_index, long a)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler70)
		{
			handlers->handler70(object_index, a);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x109140
void function_109140(long object_index, long a, long b)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler74)
		{
			handlers->handler74(object_index, a, b);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x1091b0
void function_1091b0(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler78)
		{
			handlers->handler78(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x109220
void function_109220(long object_index, long a, long b)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler7c)
		{
			handlers->handler7c(object_index, a, b);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x109290
void function_109290(long object_index, long a, long b, long c)
{
	s_object_handlers **list = OBJECT_TYPE_DEFINITION(object_index)->handlers;

	for (long i = 0; list[i]; i++)
	{
		s_object_handlers *handlers = list[i];

		if (handlers->handler68)
		{
			handlers->handler68(object_index, a, b, c);
		}
	}
}

// @retail 0x109300
void function_109300(void)
{
	long size = sizeof(s_object_list_state);
	s_object_list_state *state = (s_object_list_state *)(game_state_globals.base_address + game_state_globals.cpu_allocation_size);

	game_state_globals.cpu_allocation_size += size;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
	memset(state, 0, sizeof(s_object_list_state));
	g_5107f0 = state;
}

// @retail 0x109350
void function_109350(void)
{
	g_5107f0 = 0;
}

// @retail 0x109360
void function_109360(void)
{
	memset(g_5107f0, 0, sizeof(s_object_list_state));
	g_5107f0->locked = true;
}

// @retail 0x109380
void function_109380(void)
{
	g_5107f0->locked = false;
}

// @retail 0x109390
void function_109390(long object_index)
{
	s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xFFFF].object;

	if (TEST_FIELD_BIT(g_4e3b44[object->tag_index & 0xFFFF].flags->flag6))
	{
		if (g_5107f0->object_count < 32)
		{
			object->flags_c0 |= 8;
			g_5107f0->object_indices[g_5107f0->object_count] = object_index;
			g_5107f0->object_count++;
		}
	}
}

void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_e4c10(long object_index);

// @retail 0x109400
void __stdcall function_109400(long object_index)
{
    s_record_pool *pool = g_4e0300;
    s_object *object = ((s_object_header *)pool->data)[object_index & 0xffff].object;
    if (TEST_FIELD_BIT(g_4e3b44[object->tag_index & 0xffff].flags->flag6))
    {
        s_object_list_state *state = g_5107f0;
        for (long i = 0; i < state->object_count; ++i)
        {
            if (state->object_indices[i] == object_index)
            {
                object->flags_c0 &= 0xf7;
                state->object_indices[i] = state->object_indices[state->object_count - 1];
                --state->object_count;
                long index = NONE;
                while ((index = data_next_absolute_index_inlined(pool, index + 1)) != NONE)
                {
                    byte *header = pool->data + pool->size * index;
                    long attached_index = (*(short *)header << 16) | index;
                    byte *attached = (byte *)((s_object_header *)g_4e0300->data)[attached_index & 0xffff].object;
                    if ((header[2] & 1) && !(header[2] & 0x10) &&
                        ((*(word *)(attached + 0xc0) >> 1) & 1 || (*(word *)(attached + 0xc0) >> 2) & 1) &&
                        *(long *)(attached + 0xb8) == object_index)
                    {
                        long type_mask = 1 << header[3];
                        if (type_mask & 0x1c)
                        {
                            function_b9b90(attached_index, false);
                            function_b7360(attached_index);
                        }
                        else if (type_mask & 1)
                        {
                            function_e4c10(attached_index);
                            function_b9b90(attached_index, false);
                            function_b7360(attached_index);
                        }
                    }
                }
                break;
            }
        }
    }
}
