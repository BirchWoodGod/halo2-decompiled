// @flags /O2 /Gr
/* UNKNOWN_123680.CPP: the cache of streamed tag resources (0x123310..0x123b00) */

#include <xtl.h>
#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_123680.h"
#include "physical_memory.h"
#include "globals.h"
#include "async.h"

/* a load of a resource into its block (g_4e3b48, 0x10 bytes each, at the
   block's index) */
struct s_cache_load
{
	short salt;
	short unknown02;
	bool done;
	byte unknown05[3];
	long handle;
	s_cache_resource *resource;
};

/* a request to load a resource (g_4e3b4c, 12 bytes each) */
struct s_cache_request
{
	short salt;
	short unknown02;
	long priority;
	s_cache_resource *resource;
};

/* by request priority: the most requests, how long ago a resource of that
   priority counts as used, and how long its block stays */
struct s_cache_priority
{
	long maximum_requests;
	long age;
	long lifetime;
};

const s_cache_priority g_468810[3] =
{
	{ 8, 0, 0 },
	{ 5, 10, 30 },
	{ 2, 20, 50 },
};

extern s_physical_object *g_4e3b54;
extern dword g_4e3b50;
extern long g_4e3b58;
extern long g_4e3b5c;
s_record_pool *g_4e3b48;
s_record_pool *g_4e3b4c;
dword g_55e71c;
bool g_510c20;
bool g_510c21;

long g_55e720;

void function_1239d0(void);
long function_1234a0(long type, s_cache_resource *resource, bool flush);
void function_1235b0(s_cache_load *load, long name, long priority);
bool function_1237a0(s_cache_load *load);
void __stdcall function_1238f0(long block_index);
bool __stdcall function_1238b0(long block_index);
long function_213760(dword location, long size, void *buffer, dword *bytes_read, bool *done, long type, long priority);
bool function_120ce0(long job, long priority);

static __forceinline byte *function_123681(s_record_pool_iterator *arg_0)
{
	s_record_pool *local_0 = arg_0->data;
	long local_1 = arg_0->index + 1;
	long local_2 = NONE;
	if (local_1 >= 0)
	{
		for (; local_1 < *(long volatile *)&local_0->high_water_index; local_1++)
		{
			if (local_0->bitmap[local_1 >> 5] & (1 << (local_1 & 0x1f)))
			{
				local_2 = local_1;
				break;
			}
		}
	}
	byte *local_3;
	if (local_2 != NONE)
	{
		local_3 = local_0->data + local_0->size * local_2;
		arg_0->index = local_2;
		arg_0->datum_index = (*(short *)local_3 << 16) | local_2;
	}
	else
	{
		arg_0->index = local_0->maximum_count;
		arg_0->datum_index = NONE;
		local_3 = NULL;
	}
	return local_3;
}

// @retail 0x123680
long function_123680(s_cache_resource *resource)
{
	long result = NONE;
	if (resource->block_index == NONE)
	{
		struct
		{
			s_cache_request *field_0;
			s_record_pool_iterator field_4;
		} local_0;
		local_0.field_4.data = g_4e3b4c;
		local_0.field_4.index = NONE;
		local_0.field_4.datum_index = NONE;
		while ((local_0.field_0 = (s_cache_request *)function_123681(&local_0.field_4)) != NULL)
		{
			if (resource == local_0.field_0->resource)
			{
				result = local_0.field_4.datum_index;
				break;
			}
		}
	}
	return result;
}

// @retail 0x1236f0
void function_1236f0(s_cache_resource *resource, bool urgent)
{
	if (g_510c21)
	{
		long priority = (urgent ? 0 : 1) + 1;

		if (resource->block_index == NONE)
		{
			if (function_123680(resource) == NONE)
			{
				long request_index = record_pool_allocate(g_4e3b4c);

				if (request_index != NONE)
				{
					s_cache_request *request = &((s_cache_request *)g_4e3b4c->data)[request_index & 0xffff];

					request->priority = priority;
					request->resource = resource;
					function_1239d0();
				}
				else if (GetTickCount() > g_55e71c)
				{
					g_55e71c = GetTickCount() + 30000;
				}
			}
		}
		else
		{
			long age = g_468810[priority].age;
			long *last_used_time = &((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff].time;
			long time = g_4e3b54->time;

			if (time - *last_used_time > age)
			{
				*last_used_time = time - age;
			}
		}
	}
}

// @retail 0x1237e0
void *function_1237e0(s_cache_resource *resource, long name)
{
	long request_index = function_123680(resource);
	long block_index;
	s_physical_block *block;

	if (request_index != NONE)
	{
		record_pool_release(g_4e3b4c, request_index);
	}
	block_index = resource->block_index;
	if (block_index == NONE)
	{
		s_cache_load *load;

		block_index = function_1234a0(0, resource, true);
		load = &((s_cache_load *)g_4e3b48->data)[datum_new_at_index_with_salt(g_4e3b48, block_index) & 0xffff];
		load->handle = NONE;
		load->done = false;
		load->resource = resource;
		resource->block_index = block_index;
		function_1235b0(load, name, 0);
	}
	else
	{
		function_1237a0(&((s_cache_load *)g_4e3b48->data)[block_index & 0xffff]);
	}
	((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff].time = g_4e3b54->time;
	block = &((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff];
	return (void *)(g_4e3b50 + (block->offset << g_4e3b54->page_shift));
}

// @retail 0x123180
void function_123180(void)
{
	if (!g_510c20)
	{
		g_510c20 = true;
		g_4e3b48 = data_new("xbox animation", 0x200, sizeof(s_cache_load), 0, g_468758);
		g_4e3b4c = data_new("xbox predicted animations", 0x80, sizeof(s_cache_request), 0, g_468758);
		g_4e3b54 = physical_memory_new("xbox animation cache", 0, 0xb, 0x200, function_1238f0, function_1238b0, 0, g_468758);
		g_4e3b5c = 0;
		g_4e3b58 = 0;
		g_4e3b50 = 0;
	}
}

// @retail 0x123310
void function_123310(void)
{
	if (g_510c20)
	{
		if (g_4e3b48)
		{
			data_dispose(g_4e3b48);
			g_4e3b48 = NULL;
		}
		if (g_4e3b4c)
		{
			data_dispose(g_4e3b4c);
			g_4e3b4c = NULL;
		}
		if (g_4e3b54)
		{
			g_4e3b54->allocator->deallocate(g_4e3b54);
			g_4e3b54 = NULL;
		}
	}
	g_510c20 = false;
}

// @retail 0x1233a0
void function_1233a0(void)
{
	if (g_510c20 && !g_510c21)
	{
		g_510c21 = true;
		if (g_4e3b48)
		{
			g_4e3b48->valid = true;
			record_pool_release_all(g_4e3b48);
		}
		if (g_4e3b4c)
		{
			g_4e3b4c->valid = true;
			record_pool_release_all(g_4e3b4c);
		}
	}
}

// @retail 0x1233f0
void function_1233f0(void)
{
	if (g_510c20 && g_510c21)
	{
		physical_memory_flush(g_4e3b54);
		if (g_4e3b48)
		{
			g_4e3b48->valid = false;
		}
		if (g_4e3b4c)
		{
			g_4e3b4c->valid = false;
		}
		g_510c21 = false;
	}
}

// @retail 0x123430
void function_123430(void)
{
	if (g_510c20 && g_510c21)
	{
		s_physical_object *local_0 = g_4e3b54;
		*(long volatile *)&local_0->state = 0;
		long local_1 = *(long volatile *)&local_0->time;
		if (local_1 == 0x7fffffff)
			physical_memory_reset_time(local_0);
		else
			local_0->time = local_1 + 1;
		for (long local_2 = 0; local_2 < 8; local_2++)
			local_0->limits[local_2] = 0x7fffffff;
		function_1239d0();
		g_55e720++;
	}
}

// @retail 0x1234a0
long function_1234a0(long type, s_cache_resource *resource, bool flush)
{
	long block_index = function_13d370(g_4e3b54, resource->size, type);

	if (block_index == NONE && flush)
	{
		long attempts = 3;

		g_4e3b54->delete_proc = function_1238f0;
		g_4e3b54->busy_proc = NULL;
		while (block_index == NONE && attempts > 0)
		{
			physical_memory_new_frame(g_4e3b54);
			block_index = function_13d370(g_4e3b54, resource->size, type);
			attempts--;
		}
		g_4e3b54->delete_proc = function_1238f0;
		g_4e3b54->busy_proc = function_1238b0;
	}
	return block_index;
}

// @retail 0x123550
s_cache_load *function_123550(s_cache_resource *resource, long priority)
{
	long local_0 = resource->size;
	s_physical_object *local_1 = g_4e3b54;
	long local_2 = g_468810[priority].lifetime;
	s_cache_load *result = NULL;
	long block_index = function_13d370(local_1, local_0, local_2);

	if (block_index != NONE)
	{
		result = &((s_cache_load *)g_4e3b48->data)[datum_new_at_index_with_salt(g_4e3b48, block_index) & 0xffff];
		result->done = false;
		result->handle = NONE;
		result->resource = resource;
		resource->block_index = block_index;
	}
	return result;
}

// @retail 0x1235b0
void function_1235b0(s_cache_load *load, long name, long priority)
{
	long const *const local_1 = &name;
	s_cache_resource *resource = load->resource;
	s_physical_block *block;
	void *buffer;
	long size;

	load->done = false;
	block =&((s_physical_block *)g_4e3b54->blocks->data)[resource->block_index & 0xffff];
	buffer = (void *)(g_4e3b50 + (block->offset << g_4e3b54->page_shift));
	size = resource->size;
	if (size & 0x1ff)
	{
		size = (size | 0x1ff) + 1;
	}
	load->done = false;
	struct
	{
		long priority;
		long tag;
	} volatile read;
	read.priority = g_468810[priority].maximum_requests;
	dword local_0 = resource->unknown08;
	name = resource->unknown00;
	read.tag = *local_1;
	load->handle = function_213760(local_0, size, buffer, NULL, &load->done, 6, read.priority);
	if (priority == 0)
	{
		function_1237a0(load);
	}
	((s_physical_block *)g_4e3b54->blocks->data)[load->resource->block_index & 0xffff].time = g_4e3b54->time - g_468810[priority].age;
}

// @retail 0x1237a0
bool function_1237a0(s_cache_load *load)
{
	bool result = false;

	if (load->handle != NONE)
	{
		if (!load->done)
		{
			function_120ce0(load->handle, 8);
			function_120d50(&load->done, false);
			result = true;
		}
		load->handle = NONE;
	}
	return result;
}

// @retail 0x1238b0
bool __stdcall function_1238b0(long block_index)
{
	bool result = false;
	s_cache_load *load = &((s_cache_load *)g_4e3b48->data)[block_index & 0xffff];

	if (load->handle != NONE && !load->done)
	{
		result = true;
	}
	return result;
}

// @retail 0x1238f0
void __stdcall function_1238f0(long block_index)
{
	s_cache_load *load = &((s_cache_load *)g_4e3b48->data)[block_index & 0xffff];

	function_1237a0(load);
	load->resource->block_index = NONE;
	record_pool_release(g_4e3b48, block_index);
}

// @retail 0x123970
long function_123970(void)
{
	long count = 0;
	s_record_pool_iterator iterator;
	s_cache_load *load;

	iterator.data = g_4e3b48;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while ((load = (s_cache_load *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (!load->done)
		{
			count++;
		}
	}
	return count;
}

/* steps a datum iterator, keeping the current datum */
static __forceinline byte *datum_iterator_next_1239d0(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *result;

	if (index != NONE)
	{
		result = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)result << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		result = 0;
	}
	iterator->datum = result;
	return result;
}

// @retail 0x1239d0
void function_1239d0(void)
{
	s_record_pool *requests = g_4e3b4c;

	if (requests->valid && async_globals.tasks_added <= 25)
	{
		long loads = function_123970();
		s_record_pool_iterator iterator;
		s_data_datum_iterator next_iterator;
		s_cache_request *request;

		iterator.data = requests;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((request = (s_cache_request *)data_iterator_next_calling(&iterator)) != NULL && loads < 8)
		{
			if (request->priority == 1)
			{
				s_cache_resource *resource = request->resource;

				if (!g_510c21)
				{
					continue;
				}
				if (resource->block_index == NONE)
				{
					s_cache_load *load = function_123550(resource, 1);

					if (!load)
					{
						continue;
					}
					function_1235b0(load, 0x7000180, 1);
				}
				record_pool_release(g_4e3b4c, iterator.datum_index);
				loads++;
			}
		}

		next_iterator.data = g_4e3b4c;
		next_iterator.index = NONE;
		next_iterator.datum_index = NONE;
		while ((request = (s_cache_request *)datum_iterator_next_1239d0(&next_iterator)) != NULL && loads < 8)
		{
			long priority = request->priority;
			s_cache_resource *resource = request->resource;

			if (g_510c21)
			{
				if (resource->block_index == NONE)
				{
					s_cache_load *load = function_123550(resource, priority);

					if (!load)
					{
						continue;
					}
					function_1235b0(load, 0x7000180, priority);
				}
				record_pool_release(g_4e3b4c, next_iterator.datum_index);
				loads++;
			}
		}
	}
}
