// @flags /O2 /Ob1 /Gr
/* UNKNOWN_209580.CPP: runs a script thread if it is due (script_thread_runner; lane
   I's outside function, kept out of line as in retail, where the command
   scripts' 0x258880 calls it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* a script thread (g_4f9384, 0x418 bytes; unknown_209520.cpp) */
struct s_hs_due_thread
{
	byte unknown00[8];
	long sleep_until;
	byte unknown0c[0x418 - 0xc];
};

extern s_record_pool *g_4f9384;
extern s_record_pool *g_4f9380;
extern bool g_4f9388;

// @retail 0x209290
void function_209290(void)
{
	g_4f9384->valid = false;
	s_record_pool_iterator iterator;
	iterator.data = g_4f9380;
	iterator.index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		long datum = iterator.datum_index;
		if ((datum & 0xffff) >= 0x41d)
			record_pool_release(iterator.data, datum);
	}
	g_4f9388 = false;
}

void function_209850(long thread_index); /* unknown_209520.cpp */

/* runs the thread if it is due: 0 when it finished, 1 when it sleeps, 2 when
   it waits */
// @retail 0x209580
short function_209580(long thread_index)
{
	short result = 2;
	s_record_pool *local_0 = g_4f9384;
	s_hs_due_thread *thread = (s_hs_due_thread *)(local_0->data + (thread_index & 0xffff) * sizeof(s_hs_due_thread));

	if (thread->sleep_until == -3)
	{
		thread->sleep_until = 0;
	}
	if (thread->sleep_until >= 0 && thread->sleep_until <= g_510c54->game_time)
	{
		function_209850(thread_index);
		result = 0;
		if (thread->sleep_until != NONE)
		{
			result = (thread->sleep_until != -3) + 1;
		}
	}
	return result;
}

PRIVATE inline long next_script_record(s_record_pool *records, long index)
{
	long next = index == NONE ? 0 : (index & 0xffff) + 1;
	return data_datum_index(records, function_16bc00(records, next));
}

void object_lists_garbage_collect(void);

// @retail 0x209310
void function_209310(void)
{
	if (g_4f9388)
	{
		long current_time = g_510c54->game_time;
		s_record_pool *records = g_4f9384;
		long index = next_script_record(records, NONE);
		do
		{
			if (index == NONE)
				break;
			long absolute_index = index & 0xffff;
			s_hs_due_thread *thread = (s_hs_due_thread *)(records->data + absolute_index * sizeof(s_hs_due_thread));
			if (thread->unknown00[2] != 4 && thread->sleep_until >= 0 && thread->sleep_until <= current_time)
			{
				function_209850(index);
				records = g_4f9384;
			}
			index = data_datum_index(records, function_16bc00(records, absolute_index + 1));
		} while (g_4f9388);
		object_lists_garbage_collect();
	}
}

// @retail 0x209e70
long function_209e70(short script_index)
{
	s_record_pool *records = g_4f9384;
	for (long index = next_script_record(records, NONE); index != NONE; index = next_script_record(records, index))
	{
		s_hs_due_thread *thread = (s_hs_due_thread *)(records->data + (index & 0xffff) * sizeof(s_hs_due_thread));
		if (*(long *)&thread->unknown00[4] == script_index)
			return index;
	}
	return NONE;
}

extern s_record_pool *g_4f9394;

int function_11c920(char const *left, char const *right);

struct s_named_thread
{
	byte unknown00[4];
	long script_index;
	byte unknown08[0x418 - 8];
};

struct s_script_name_entry
{
	char name[32];
	byte unknown20[8];
};

struct s_script_name_table
{
	byte unknown000[0x1bc];
	s_script_name_entry *entries;
};

// @retail 0x209f00
long __stdcall function_209f00(char const *name)
{
	s_record_pool *records = g_4f9384;
	long index = data_datum_index(records, function_16bc00(records, 0));
	while (index != NONE)
	{
		s_named_thread *thread = &((s_named_thread *)records->data)[index & 0xffff];
		long script_index = thread->script_index;
		if (script_index != NONE)
		{
			char const *script_name = ((s_script_name_table *)g_4e0350)->entries[script_index].name;
			if (!function_11c920(script_name, name))
				return index;
			records = g_4f9384;
		}
		long next = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(records, data_next_absolute_index_inlined(records, next));
	}
	return NONE;
}

extern s_record_pool *g_4f55d8;
extern s_record_pool *g_4f55d4;
void function_20a850(void);

PRIVATE __forceinline void script_pool_disconnect(s_record_pool *data)
{
	if (!data->flag1)
	{
		data->flag1 = true;
		data->data = NULL;
		data->valid = false;
	}
}

// @retail 0x20a7d0
void function_20a7d0(void)
{
	function_20a850();
	script_pool_disconnect(g_4f9394);
	function_209290();
	g_4f55d8->valid = false;
	g_4f55d4->valid = false;
}

// @retail 0x20a850
void function_20a850(void)
{
	if (g_4f9388)
	{
		s_record_pool *threads = g_4f9384;
		for (long index = next_script_record(threads, NONE); index != NONE; index = next_script_record(threads, index))
		{
			byte *thread = threads->data + (index & 0xffff) * 0x418;
			if (thread[2] == 2)
				record_pool_release(threads, index);
		}
	}
	s_record_pool *expressions = g_4f9394;
	for (long index = next_script_record(expressions, NONE); index != NONE; )
	{
		byte *expression = expressions->data + (index & 0xffff) * 20;
		if (!(expression[6] & 8))
			record_pool_release(expressions, index);
		long next = index == NONE ? 0 : (index & 0xffff) + 1;
		index = data_datum_index(expressions, data_next_absolute_index_inlined(expressions, next));
	}
}


extern long g_4686c0;
void function_2a09c0(void);
void function_2090a0(void);

// @retail 0x20a750
void function_20a750(void)
{
    byte *scenario = (byte *)g_4e0350;
    if (g_4686c0 == NONE ? NULL : scenario)
    {
        if (g_4f9394->flag1)
            function_16b6b0(g_4f9394, *(long *)(scenario + 0x238), *(byte **)(scenario + 0x23c));
        if (*(long *)(scenario + 0x1b8) || *(long *)(scenario + 0x1d0) <= 0)
            function_2a09c0();
    }
    g_4f55d8->valid = true;
    record_pool_release_all(g_4f55d8);
    g_4f55d4->valid = true;
    record_pool_release_all(g_4f55d4);
    function_2090a0();
}


void function_20a1a0(void);
long function_16b990(s_record_pool *data, long index);

// @retail 0x208ff0
void function_208ff0(void)
{
    g_4f9384 = data_new_inlined("hs thread", 0x140, 0x418, 0, g_510c2c);
    g_4f9380 = data_new_inlined("hs globals", 0xc00, 8, 0, g_510c2c);
    if (g_4f9384 && g_4f9380)
    {
        g_4f9380->valid = true;
        record_pool_release_all(g_4f9380);
        for (long i = 0; i < 0x41d; i++)
            function_16b990(g_4f9380, i);
    }
    function_20a1a0();
}


void function_1ded00(void);


PRIVATE inline s_record_pool *script_expression_pool_new(char const *name, long count, long size, c_data_allocator *allocator)
{
    long bitmap_size = ((count + 31) >> 5) * sizeof(dword);
    s_record_pool *expressions = (s_record_pool *)allocator->allocate(sizeof(s_record_pool) + bitmap_size);
    if (expressions)
        data_array_construct(expressions, name, count, size, 0, allocator, (dword *)(expressions + 1));
    return expressions;
}

// @retail 0x20a700
void function_20a700(void)
{
    g_4f9394 = script_expression_pool_new("script node", 0x9000, 20, g_468758);
    function_1ded00();
    function_208ff0();
    function_20a750();
}
