// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_22C0A0.CPP: the fixed buffer allocator Havok's memory uses
   (vtable 0x4576a4; unknown_146a20.h): allocations grow up from the buffer's
   start and their table (an offset and a size each) down from its end */

#include <string.h>
#include "unknown_11c920.h"
#include "unknown_146a20.h"

struct s_fixed_memory_entry
{
	long offset;
	long size;
};

// @retail 0x22c330
void *c_havok_fixed_memory::function_22c330(long alignment, long limit,
	long offset, long size, s_fixed_memory_entry *entry)
{
	void *buffer = m_buffer;
	void *result = NULL;
	if (limit >= offset)
	{
		switch (alignment)
		{
		case 2: offset = (offset + 15) & ~15; break;
		case 4: offset = (offset + 15) & ~15; break;
		case 8: offset = (offset + 15) & ~15; break;
		case 16: offset = (offset + 15) & ~15; break;
		case 0: break;
		case 1: break;
		default: __assume(0);
		}
		if (limit >= size + offset)
		{
			entry->offset = offset;
			entry->size = size;
			result = (byte *)buffer + offset;
		}
	}
	return result;
}

// @retail 0x22c0a0
void *c_havok_fixed_memory::allocate(long size, long memory_class)
{
	return allocate_aligned(1, size, memory_class);
}

// @retail 0x22c0c0
void c_havok_fixed_memory::deallocate(void *address)
{
	deallocate_aligned(address);
}

/* drops the allocation's entry from the table */
// @retail 0x22c280
void c_havok_fixed_memory::deallocate_aligned(void *address)
{
	for (long i = 0; i < m_count; i++)
	{
		if ((byte *)m_buffer + ((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-1 - i].offset == address)
		{
			for (long j = i; j < m_count - 1; j++)
			{
				((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-1 - j] = ((s_fixed_memory_entry *)((byte *)m_buffer + m_size))[-2 - j];
			}
			m_count--;
			return;
		}
	}
}

// @retail 0x22c300
void *c_havok_fixed_memory::allocate16(long size, long memory_class)
{
	return allocate_aligned(16, size, memory_class);
}

// @retail 0x22c320
void c_havok_fixed_memory::deallocate16(void *address, long size, long memory_class)
{
	deallocate_aligned(address);
}

/* what the fixed buffers report: the header's size, the bytes allocated and
   the allocations */
struct s_fixed_memory_statistics
{
	long header_size;
	long allocated_size;
	long unknown08;
	long allocation_count;
	long unknown10;
};

struct s_fixed_memory_statistics_16
{
	long header_size;
	long allocated_size;
	long allocation_count;
	long unknown0c;
};

// @retail 0x22cbd0
void fixed_memory_get_statistics(c_havok_fixed_memory *memory, s_fixed_memory_statistics *statistics)
{
	if (memory)
	{
		statistics->header_size = 0x30;
		statistics->allocation_count = memory->m_count;
		statistics->allocated_size = 0;
		for (long i = 0; i < memory->m_count; i++)
		{
			s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)memory->m_buffer + memory->m_size);

			statistics->allocated_size += entries[-1 - i].size;
		}
		statistics->unknown10 = 0;
	}
	else
	{
		memset(statistics, 0, sizeof(*statistics));
	}
}

// @retail 0x22cc30
void fixed_memory_get_statistics_16(c_havok_fixed_memory *memory, s_fixed_memory_statistics_16 *statistics)
{
	if (memory)
	{
		statistics->header_size = 0x30;
		statistics->allocation_count = memory->m_count;
		statistics->allocated_size = 0;
		for (long i = 0; i < memory->m_count; i++)
		{
			s_fixed_memory_entry *entries = (s_fixed_memory_entry *)((byte *)memory->m_buffer + memory->m_size);

			statistics->allocated_size += entries[-1 - i].size;
		}
		statistics->allocation_count = memory->m_count;
		statistics->unknown0c = 0;
	}
	else
	{
		memset(statistics, 0, sizeof(*statistics));
	}
}


// @retail 0x22c0d0
void *c_havok_fixed_memory::allocate_aligned(long alignment, long size, long memory_class)
{
    void *result = NULL;
    long used = 0;
    long offset = 0;
    s_fixed_memory_entry entry;
    if (m_count > 0)
    {
        s_fixed_memory_entry *last = (s_fixed_memory_entry *)((byte *)m_buffer + m_size - m_count * 8);
        used = last->offset + last->size;
    }
    if (used < m_size - (m_count * 8 + 8))
    {
        long i;
        for (i = 0; i < m_count; ++i)
        {
            s_fixed_memory_entry *current = (s_fixed_memory_entry *)((byte *)m_buffer + m_size - (i * 8 + 8));
            long limit = current->offset;
            if (limit >= size + offset)
            {
                result = function_22c330(alignment, limit, offset, size, &entry);
                if (result)
                {
                    m_count++;
                    for (long j = m_count - 1; j > i; --j)
                    {
                        s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                        items[-j - 1] = items[-j];
                    }
                    s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                    items[-i - 1] = entry;
                    goto done;
                }
            }
            offset = limit + current->size;
        }
        if (!result)
        {
            long limit = m_size - (m_count * 8 + 8);
            if (limit >= offset)
            {
                switch (alignment)
                {
                case 0: break;
                case 1: break;
                case 2: offset = (offset + 15) & ~15; break;
                case 4: offset = (offset + 15) & ~15; break;
                case 8: offset = (offset + 15) & ~15; break;
                case 16: offset = (offset + 15) & ~15; break;
                default: __assume(0);
                }
                if (limit >= size + offset) result = (byte *)m_buffer + offset;
            }
            if (result)
            {
                m_count++;
                s_fixed_memory_entry *items = (s_fixed_memory_entry *)((byte *)m_buffer + m_size);
                items[-m_count].offset = offset;
                items[-m_count].size = size;
            }
        }
    }
done:
    return result;
}
