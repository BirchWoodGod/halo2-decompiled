// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <string.h>

struct s_widget_quad_2b11;
struct s_float_rect;
struct s_bitmap_widget_22ae21;
struct s_widget_transform_block;
struct s_510c4c;
extern s_510c4c *g_510c4c;
struct s_4e6950
{
	byte field_0[0x40];
	color4f field_40;
	color3f field_50, field_5c, field_68;
	byte field_74[0xc];
};
extern s_4e6950 g_4e6950;
long function_13a690(long);
void function_22ae21(long, s_bitmap_widget_22ae21 const *, long *, long *, long *, box2f *);
void function_1396c7(long, point2f *);
void function_22bcaf(s_widget_transform_block const *, point2f *, point2f *, point2f *);
double __stdcall function_1d6d0(real);
double __cdecl function_2b460(real);
void function_22a664(s_widget_quad_2b11 const *, s_float_rect const *, long, long, long);
color3f *unpack_color3f(dword, color3f *);
bool function_22acb4(long);
void function_1392c5(long);

PRIVATE __forceinline real function_22aff2(real arg_0)
{
	return arg_0 < 0.0f ? 0.0f : arg_0 > 1.0f ? 1.0f : arg_0;
}

struct s_22aff3
{
    point2f field_0;
    real field_8;
    point2f field_c[4];
    union
    {
        struct { byte field_2c[4]; point2f field_30; };
        color3f field_2c_1;
    };
    union { long field_38; real field_38_1; };
    point2f field_3c;
    long field_44;
    point2f field_48;
    union { long field_50; real field_50_1; dword const *field_50_2; };
    box2f field_54;
    byte field_64[3];
    bool field_67;
};

PRIVATE __forceinline real function_22aff5(real arg_0)
{
    real local_0 = arg_0;
    if (0.0f > arg_0)
        local_0 = 0.0f;
    else if (arg_0 > 1.0f)
        local_0 = 1.0f;
    return local_0;
}

#pragma intrinsic(memcpy)
// @retail 0x22aff1
void function_22aff1(long arg_0, byte const *arg_1, byte const *arg_2, real const *arg_3)
{
	long local_0 = function_13a690(*(word const *)(arg_2 + 0x1c));
	(void)&arg_0; (void)&arg_1; (void)&arg_2; (void)&arg_3;
	if (*(long const *)(arg_2 + 0x2c) != NONE && *(long const *)(arg_2 + 0x24) != NONE)
	{
		s_22aff3 local_45;
		function_22ae21(arg_0, (s_bitmap_widget_22ae21 const *)arg_2,
			&local_45.field_44, &local_45.field_50, &local_45.field_38, &local_45.field_54);
		if (local_45.field_44 != NONE)
		{
			local_45.field_67 = false;
			function_1396c7(*(word const *)(arg_2 + 0x1c), &local_45.field_48);
			local_45.field_0.x = 1.0f; local_45.field_0.y = 0.0f;
			local_45.field_30.x = 0.0f; local_45.field_30.y = 1.0f;
			switch (local_0)
			{
			case 0:
				local_45.field_48.x += *(short const *)(arg_2 + 0x34);
				local_45.field_48.y += *(short const *)(arg_2 + 0x36);
				local_45.field_3c.x = local_45.field_54.x0 + (local_45.field_54.x1 - local_45.field_54.x0) * *(real const *)(arg_2 + 0x40);
				local_45.field_3c.y = local_45.field_54.y0 + (local_45.field_54.y1 - local_45.field_54.y0) * *(real const *)(arg_2 + 0x44);
				break;
			case 1:
				local_45.field_48.x += *(short const *)(arg_2 + 0x38);
				local_45.field_48.y += *(short const *)(arg_2 + 0x3a);
				local_45.field_3c.x = local_45.field_54.x0 + (local_45.field_54.x1 - local_45.field_54.x0) * *(real const *)(arg_2 + 0x48);
				local_45.field_3c.y = local_45.field_54.y0 + (local_45.field_54.y1 - local_45.field_54.y0) * *(real const *)(arg_2 + 0x4c);
				break;
			case 2:
				local_45.field_48.x += *(short const *)(arg_2 + 0x3c);
				local_45.field_48.y += *(short const *)(arg_2 + 0x3e);
				local_45.field_3c.x = local_45.field_54.x0 + (local_45.field_54.x1 - local_45.field_54.x0) * *(real const *)(arg_2 + 0x50);
				local_45.field_3c.y = local_45.field_54.y0 + (local_45.field_54.y1 - local_45.field_54.y0) * *(real const *)(arg_2 + 0x54);
				break;
			}
			g_4e6950.field_40 = *(color4f const *)arg_3;
			function_22bcaf((s_widget_transform_block const *)(arg_2 + 0x58), &local_45.field_48, &local_45.field_0, &local_45.field_30);
			long local_44 = local_45.field_50;
			local_45.field_8 = (real)local_45.field_38;
			local_45.field_38_1 = (real)local_45.field_50;
			for (long local_13 = 0; local_13 < 4; ++local_13)
			{
				bool local_14 = local_13 > 1;
				bool local_15 = local_13 == 1 || local_13 == 2;
				local_45.field_50_1 = local_14 ? (real)function_1d6d0((local_45.field_54.y1 - local_45.field_3c.y) * local_45.field_8) :
					(real)-function_2b460((local_45.field_3c.y - local_45.field_54.y0) * local_45.field_8);
				real &local_17 = *(real *)&arg_3;
				local_17 = local_15 ? (real)function_1d6d0((local_45.field_54.x1 - local_45.field_3c.x) * local_45.field_38_1) :
					(real)-function_2b460((local_45.field_3c.x - local_45.field_54.x0) * local_45.field_38_1);
				local_45.field_c[local_13].x = local_45.field_0.x * local_17 + local_45.field_30.x * local_45.field_50_1 + local_45.field_48.x;
				local_45.field_c[local_13].y = local_45.field_0.y * local_17 + local_45.field_30.y * local_45.field_50_1 + local_45.field_48.y;
			}
			if (*(word const *)(arg_2 + 0x1e) & 1)
			{
				real local_18 = local_45.field_54.x0;
				local_45.field_54.x0 = local_45.field_54.x1;
				local_45.field_54.x1 = local_18;
			}
			if (*(word const *)(arg_2 + 0x1e) & 2)
			{
				real local_19 = local_45.field_54.y0;
				local_45.field_54.y0 = local_45.field_54.y1;
				local_45.field_54.y1 = local_19;
			}
			switch (*(word const *)(arg_2 + 0x60))
			{
			case 1:
				function_1392c5(*(long const *)(arg_1 + 0x188));
				g_4e6950.field_50 = *(color3f const *)(arg_1 + 0x18c);
				break;
			case 2:
				function_1392c5(*(long const *)(arg_1 + 0x1d4));
				g_4e6950.field_50 = *(color3f const *)(arg_1 + 0x1d8);
				break;
			case 3:
				g_4e6950.field_50 = *(color3f const *)(arg_1 + 0x18c);
				g_4e6950.field_5c = *(color3f const *)(arg_1 + 0x18c);
				break;
			case 4:
				g_4e6950.field_50 = *(color3f const *)(arg_1 + 0x1d8);
				g_4e6950.field_5c = *(color3f const *)(arg_1 + 0x1d8);
				break;
			case 5:
			{
				byte const *local_20 = (byte const *)g_510c4c + arg_0 * 0x6c;
				dword local_21[5] = {0, 0xff0000, 0xff00, 0xffff00, 0x7f00ff};
				local_45.field_67 = true;
				local_45.field_50_2 = local_21;
				for (long local_22 = 0; local_22 <= 4; ++local_45.field_50_2, ++local_22)
				{
					bool local_47 = local_22 == 0;
					real local_23 = function_22aff5(*(real const *)(arg_1 + 0x2c) - local_22);
					real local_24 = function_22aff5(*(real const *)local_20 - local_22);
					bool local_25 = local_24 > local_23;
					real local_26 = local_25 ? *(real const *)(local_20 + 4) : -1.0f;
					real local_27 = local_26 < 0.0f ? 0.0f : function_22aff5(1.0f - local_26);
					color3f &local_28 = local_45.field_2c_1;
					local_28 = *(color3f const *)g_468710;
					local_28.red *= local_27;
					local_28.green *= local_27;
					local_28.blue *= local_27;
					if (!local_25)
						local_24 = local_23;
					if (local_23 <= 0.0f && local_24 <= 0.0f)
						break;
					g_4e6950.field_40.alpha = local_24;
					g_4e6950.field_40.red = local_23;
					g_4e6950.field_50 = local_28;
					if (local_47)
					{
						if (function_22acb4(arg_0))
						{
							unpack_color3f(0x45059a, &g_4e6950.field_5c);
							unpack_color3f(0x9c46c1, &g_4e6950.field_68);
						}
						else
						{
							unpack_color3f(0x55aa, &g_4e6950.field_5c);
							unpack_color3f(0x78f0, &g_4e6950.field_68);
						}
					}
					else
					{
						unpack_color3f(*local_45.field_50_2, &g_4e6950.field_5c);
						unpack_color3f(*local_45.field_50_2, &g_4e6950.field_68);
					}
					function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
						*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
				}
				break;
			}
			case 7:
			{
				local_45.field_67 = true;
				long local_29 = *(long const *)(arg_1 + 0x22c);
				if (local_29 > 0)
				{
					real local_30 = (real)(local_44 + 1);
					real local_31 = (real)(local_29 - 1) * (local_30 * 0.5f);
					for (long local_32 = 0; local_32 < 4; ++local_32)
						local_45.field_c[local_32].x -= local_31;
					g_4e6950.field_5c = *(color3f const *)g_468718;
					for (long local_33 = 0; local_33 < *(long const *)(arg_1 + 0x22c); ++local_33)
					{
						unpack_color3f(*(dword const *)(arg_1 + 0x230 + local_33 * 4), &g_4e6950.field_50);
						g_4e6950.field_40.alpha = *(real const *)(arg_1 + 0x250 + local_33 * 4);
						function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
							*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
						for (long local_34 = 0; local_34 < 4; ++local_34)
							local_45.field_c[local_34].x += local_30;
					}
				}
				break;
			}
			}
			if (*(word const *)(arg_2 + 0x1e) & 0x10)
			{
				real local_35 = local_45.field_c[1].x - local_45.field_c[0].x;
				real local_36 = local_45.field_c[2].y - local_45.field_c[1].y;
				for (long local_37 = 0; local_37 < 4; ++local_37)
				{
					bool local_46 = local_37 > 1;
					if (local_37 != 1 && local_37 != 2)
						local_45.field_c[local_37].x -= local_35 * 10.0f;
					if (!local_46)
						local_45.field_c[local_37].y -= local_36 * 10.0f;
				}
				local_45.field_54.x0 -= (local_45.field_54.x1 - local_45.field_54.x0) * 10.0f;
				local_45.field_54.y0 -= (local_45.field_54.y1 - local_45.field_54.y0) * 10.0f;
			}
			if (!local_45.field_67)
			{
				function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
					*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
				real local_38 = local_45.field_c[1].x - local_45.field_c[0].x;
				real local_39 = local_45.field_c[2].y - local_45.field_c[1].y;
				point2f local_40[4];
				memcpy(local_40, local_45.field_c, sizeof(local_40));
				if (*(word const *)(arg_2 + 0x1e) & 4)
				{
					for (long local_41 = 0; local_41 < 4; ++local_41)
						if (local_41 != 1 && local_41 != 2)
							local_45.field_c[local_41].x += local_38 * 2.0f;
					function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
						*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
				}
				memcpy(local_45.field_c, local_40, sizeof(local_40));
				if (*(word const *)(arg_2 + 0x1e) & 8)
				{
					for (long local_42 = 0; local_42 < 4; ++local_42)
						if (local_42 <= 1)
							local_45.field_c[local_42].y += local_39 * 2.0f;
					function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
						*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
				}
				memcpy(local_45.field_c, local_40, sizeof(local_40));
				if ((*(word const *)(arg_2 + 0x1e) & 4) && (*(word const *)(arg_2 + 0x1e) & 8))
				{
					for (long local_43 = 0; local_43 < 4; ++local_43)
					{
						bool local_46 = local_43 > 1;
						if (local_43 != 1 && local_43 != 2)
							local_45.field_c[local_43].x += local_38 * 2.0f;
						if (!local_46)
							local_45.field_c[local_43].y += local_39 * 2.0f;
					}
					function_22a664((s_widget_quad_2b11 const *)local_45.field_c, (s_float_rect const *)&local_45.field_54,
						*(long const *)(arg_2 + 0x24), local_45.field_44, *(long const *)(arg_2 + 0x2c));
				}
			}
		}
	}
}
#pragma function(memcpy)
