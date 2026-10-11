#include "unknown_11c920.h"
#include "globals.h"
#include "hs.h"

// @flags /O2 /Gr

/* the evaluation threads (g_4f9384, 0x418 bytes each) keep a stack of
   frames; each frame names a definition (g_4f9394, 20 bytes each) and, in
   the frame below it, the slot that receives the value computed */
struct s_frame
{
	s_frame *next;
	long definition_index;
	long *result;
};

struct s_thread
{
	byte unknown00[0x10];
	s_frame *frame;
	byte unknown14[0x418 - 0x14];
};

struct s_definition
{
	byte unknown00[2];
	short index;
	short type;
	byte flags;
	byte unknown07[0x14 - 7];
};

struct s_type_entry
{
	byte unknown00[0x22];
	short type;
	byte unknown24[0x28 - 0x24];
};

struct s_type_globals
{
	byte unknown00[0x1bc];
	s_type_entry *types;
};

typedef long (__stdcall *t_convert_proc)(long value);

s_record_pool *g_4f9384;
extern s_record_pool *g_4f9394;
s_type_f4462a *g_4744e0[1];
t_convert_proc g_4f5770[0x3e * 0x3e];

long function_bb760(short index);

// @retail 0x209ae0
void function_209ae0(long thread_handle, long value)
{
	long thread_index = thread_handle & 0xffff;
	s_thread *thread = &((s_thread *)g_4f9384->data)[thread_index];
	short expected;
	s_definition *definition = &((s_definition *)g_4f9394->data)[thread->frame->definition_index & 0xffff];
	short actual;

	if (!(definition->flags & 2))
		expected = g_4744e0[definition->index]->return_type;
	else
		expected = ((s_type_globals *)g_4e0350)->types[definition->index].type;
	actual = definition->type;

	if (expected != actual && expected != 3)
	{
		if (actual < 0x38)
		{
			if (actual >= 0x32 && actual <= 0x37)
			{
				if (expected >= 0x38 && expected <= 0x3d)
					value = function_bb760((short)value);
			}
			else
			{
				value = g_4f5770[actual * 0x3e + expected](value);
			}
		}
		else if (actual > 0x3d)
		{
			if (actual >= 0x32 && actual <= 0x37)
			{
				if (expected >= 0x38 && expected <= 0x3d)
					value = function_bb760((short)value);
			}
			else
			{
				value = g_4f5770[actual * 0x3e + expected](value);
			}
		}
	}

	*thread->frame->next->result = value;
	thread = &((s_thread *)g_4f9384->data)[thread_index];
	thread->frame = thread->frame->next;
}
