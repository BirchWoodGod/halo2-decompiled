#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

struct s_13c051
{
	short field_0;
	short field_2;
	short field_4;
	short field_6;
};

struct s_13c052
{
	point2f field_0;
	point2f field_8;
	dword field_10;
};

struct s_13c050
{
	dword field_0;
	byte field_4[8];
	byte *field_c;
	byte field_10[0x18];
	real field_28;
	real field_2c;
	byte field_30[0x10];
	real field_40;
	real field_44;
	byte field_48[0x48];
	__int64 field_90;
};

void __stdcall function_52040(void const *arg_1, void const *arg_2);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x13c050
void function_13c050(s_13c051 const *arg_2, dword arg_1)
{
	if (g_4e0350)
	{
		byte *local_1 = *(long *)((byte *)g_4e034c + 0x108) ? *(byte **)((byte *)g_4e034c + 0x10c) : NULL;
		byte *local_2 = g_4e3b44[*(long *)(local_1 + 0x64) & 0xffff].bytes;
		real first_x = (real)arg_2->field_2;
		long first_y = arg_2->field_0;
		_ReadWriteBarrier();
		byte *local_3 = *(byte **)(local_2 + 0x48);
		point2f local_4[4];
		local_4[0].x = first_x;
		local_4[0].y = first_y;
		local_4[1].x = arg_2->field_6;
		local_4[1].y = first_y;
		local_4[2].x = arg_2->field_6;
		local_4[2].y = arg_2->field_4;
		local_4[3].x = first_x;
		local_4[3].y = arg_2->field_4;
		s_13c052 local_5[4];
		local_5[0].field_0 = local_4[0];
		local_5[1].field_0 = local_4[1];
		local_5[0].field_8.x = 0.0f;
		local_5[0].field_8.y = 0.0f;
		local_5[1].field_8.x = 0.0f;
		local_5[1].field_8.y = 0.0f;
		local_5[2].field_8.x = 0.0f;
		local_5[2].field_8.y = 0.0f;
		local_5[3].field_8.x = 0.0f;
		local_5[3].field_8.y = 0.0f;
		local_5[0].field_10 = arg_1;
		local_5[1].field_10 = arg_1;
		local_5[2].field_10 = arg_1;
		local_5[2].field_0 = local_4[2];
		local_5[3].field_10 = arg_1;
		local_5[3].field_0 = local_4[3];
		s_13c050 local_6;
		memset(&local_6, 0, sizeof(local_6));
		local_6.field_c = local_3 + 0x74;
		*(word *)((byte *)&local_6 + 0x94) = 0;
		local_6.field_0 = 0;
		*((byte *)&local_6 + 0x96) = false;
		local_6.field_44 = 1.0f;
		local_6.field_40 = 1.0f;
		local_6.field_2c = 1.0f;
		local_6.field_28 = 1.0f;
		function_52040(&local_6, local_5);
	}
}
#pragma function(_ReadWriteBarrier)
