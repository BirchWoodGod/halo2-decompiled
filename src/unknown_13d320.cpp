// @flags /O2 /Gr
/* UNKNOWN_13D320.CPP: the physical allocator's choice between two blocks */

#include "unknown_11c920.h"

struct s_physical_block_view
{
	byte unknown00[4];
	dword time;
	byte unknown08[4];
	long size;
};

struct s_physical_allocator_view
{
	byte unknown00[0x38];
	dword time;
};

/* whether block a should go before block b: the one used longer ago (by more
   than 150 ticks, up to 300), else the larger */
#pragma auto_inline(off)
// @retail 0x13d320
bool function_13d320(s_physical_allocator_view const *allocator, s_physical_block_view const *a, s_physical_block_view const *b)
{
	bool result = false;
	dword age_a = allocator->time - a->time;
	if (age_a > 300)
	{
		age_a = 300;
	}
	dword age_b = allocator->time - b->time;
	if (age_b > 300)
	{
		age_b = 300;
	}

	if ((long)(age_a + 150) < (long)age_b)
	{
		return false;
	}
	if ((long)age_a > (long)(age_b + 150) || a->size < b->size)
	{
		result = true;
	}
	return result;
}
#pragma auto_inline(on)
