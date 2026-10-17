/* UNKNOWN_146A20.H: the memory Havok allocates from (src/unknown_146a20.cpp).
   The game builds its allocators in place, in one block of virtual memory
   (g_479888): a fixed buffer (0x15000 bytes), a Havok pool, and two more
   fixed buffers made on demand, one in the ai's scratch buffers and one in
   physical memory. */

#ifndef UNKNOWN_146A20_H
#define UNKNOWN_146A20_H

#include "unknown_11c920.h"

/* the reference count Havok's objects start with */
struct hkMemoryBase
{
	hkMemoryBase()
	{
		m_unknown04 = 1;
	}

	long m_unknown04;
};

/* Havok's memory interface (vtable 0x453760; its functions are pure) */
class hkMemory : public hkMemoryBase
{
public:
	/* three zeros, then NONE; as loops, the stores stay in a block of
	   their own ahead of the derived constructors' (0x146b80) */
	hkMemory()
	{
		long i;
		for (i = 0; i < 3; i++)
		{
			m_unknown08[i] = 0;
		}
		for (; i < 4; i++)
		{
			m_unknown08[i] = NONE;
		}
	}

	virtual void *allocate(long size, long memory_class) = 0;
	virtual void deallocate(void *address) = 0;
	virtual void *allocate_aligned(long alignment, long size, long memory_class) = 0;
	virtual void deallocate_aligned(void *address) = 0;
	virtual void *allocate16(long size, long memory_class) = 0;
	virtual void deallocate16(void *address, long size, long memory_class) = 0;
	virtual void method6(long unknown) = 0;
	virtual void method7(long unknown) = 0;
	virtual ~hkMemory() {}
	/* library functions (0x2d7020, 0x2d7090) */
	virtual void method9(void) {}
	virtual void method10(void) {}

	long m_unknown08[4];
};

/* Havok's pool memory (vtable 0x457620; constructor 0x22c3e0) */
class hkPoolMemory : public hkMemory
{
public:
	hkPoolMemory();

	virtual void *allocate(long size, long memory_class) { return 0; }
	virtual void deallocate(void *address) {}
	virtual void *allocate_aligned(long alignment, long size, long memory_class) { return 0; }
	virtual void deallocate_aligned(void *address) {}
	virtual void *allocate16(long size, long memory_class) { return 0; }
	virtual void deallocate16(void *address, long size, long memory_class) {}
	virtual void method6(long unknown) {}
	virtual void method7(long unknown) {}

	/* the fraction of the pool's blocks in use (0x22cb90) */
	real get_used_fraction(void);

	byte unknown18[0xf0340 - 0x18];
	long page_count;
	byte unknownf0344[0xf03d0 - 0xf0344];
};

/* a fixed buffer: allocations grow up from its start, and the table of
   them (8 bytes each) down from its end (vtable 0x4576a4; its functions
   are at 0x22c0a0..0x22c330) */
class c_havok_fixed_memory_base : public hkMemory
{
public:
	c_havok_fixed_memory_base(void *buffer, long size)
	{
		m_buffer = buffer;
		m_size = size & 0x0ffffffc;
		m_count = 0;
	}

	void *m_buffer;
	long m_size;
	long m_count;
	byte unknown24[0x30 - 0x24];
};

struct s_fixed_memory_entry;

class c_havok_fixed_memory : public c_havok_fixed_memory_base
{
public:
	c_havok_fixed_memory(void *buffer, long size) :
		c_havok_fixed_memory_base(buffer, size)
	{
	}

	__declspec(noinline) void *function_22c330(long alignment, long limit, long size, long offset, s_fixed_memory_entry *entry);

	/* unknown_22c0a0.cpp */
	virtual void *allocate(long size, long memory_class);
	virtual void deallocate(void *address);
	virtual void *allocate_aligned(long alignment, long size, long memory_class);
	virtual void deallocate_aligned(void *address);
	virtual void *allocate16(long size, long memory_class);
	virtual void deallocate16(void *address, long size, long memory_class);
	virtual void method6(long unknown) {}
	virtual void method7(long unknown) {}
};

#endif
