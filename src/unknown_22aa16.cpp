// @flags /O1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"

struct s_text_widget;
struct s_text_widget_state;
struct s_widget_transform_block;
struct s_interface_function_context;
struct s_33a0b_default;
extern s_interface_function_context g_5021d0;
extern s_33a0b_default *g_4686d4;
struct s_4e6950;
extern s_4e6950 g_4e6950;
class c_1fa50
{
public:
	void function_1fb10(short_rectangle2d const *, real) const;
};
void function_22a871(s_text_widget const *, s_text_widget_state const *, color4f const *, word *, long *);
void function_13edb0(long, long, long, dword, color4f const *, color4f const *);
bool function_13ee20(word const *, long);
long function_13a690(long);
void function_1be50(void);
void function_1bd50(void *);
void function_1bbf0(long, long, long, long, long, real);
void function_1cf50(void);
color3f *unpack_color3f(dword, color3f *);
void function_1396c7(long, point2f *);
void function_22bcaf(s_widget_transform_block const *, point2f *, point2f *, point2f *);
void function_13e9c0(word const *, short_rectangle2d const *, short_rectangle2d *, short_rectangle2d *, real);
void function_13eb60(color4f const *);
void function_13ed50(color4f const *);

struct s_22aa17
{
    word field_0[256];
    union
    {
        short_rectangle2d field_200;
        byte field_0200[12];
    };
    point2f field_20c;
    union
    {
        point2f field_214;
        short_rectangle2d field_0214;
    };
    union
    {
        point2f field_21c;
        struct { long field_0, field_4; } field_021c;
    };
};

#pragma optimize("y", off)
// retail keeps all four arguments on the stack (ret 0x10); without the marker LTCG passes the first in a register
// @retail 0x22aa16 standard
void __stdcall function_22aa16(long arg_0, byte const *arg_1, byte const *arg_2, real const *arg_3)
{
	(void)&arg_0; (void)&arg_1; (void)&arg_2; (void)&arg_3;
	if (*(long const *)(arg_2 + 0x28) && *(long const *)(arg_2 + 0x24) != NONE)
	{
		s_22aa17 local_10;
		{
		local_10.field_021c.field_4 = NONE;
		local_10.field_0[0] = 0;
		function_22a871((s_text_widget const *)arg_2, (s_text_widget_state const *)arg_1,
			(color4f const *)arg_3, local_10.field_0, &local_10.field_021c.field_4);
		function_13edb0(local_10.field_021c.field_4, NONE, 0, 0, g_4686cc, (color4f const *)g_4686d4);
		}
		if (function_13ee20(local_10.field_0, g_4e73a0.font))
		{
			long local_2 = function_13a690(*(word const *)(arg_2 + 0x1c));
			function_1be50();
			function_1bd50(0);
			function_1bd50(&g_5021d0);
			unpack_color3f(*(dword const *)((byte const *)g_510c94 + 0xa4), (color3f *)&g_4e6950);
			function_1bbf0(*(long const *)(arg_2 + 0x24), 0, 1, 0, 0, 100.0f);
			function_1cf50();
			g_4e73a0.unknown5e = 0;
			g_4e73a0.unknown60 = 0;
			
			local_10.field_214.x = 1.0f;
			local_10.field_214.y = 0.0f;
			local_10.field_20c.x = 0.0f;
			local_10.field_20c.y = 1.0f;
			function_1396c7(*(word const *)(arg_2 + 0x1c), &local_10.field_21c);
			switch (local_2)
			{
			case 0:
				local_10.field_21c.x += *(short const *)(arg_2 + 0x40);
				local_10.field_21c.y += *(short const *)(arg_2 + 0x42);
				break;
			case 1:
				local_10.field_21c.x += *(short const *)(arg_2 + 0x44);
				local_10.field_21c.y += *(short const *)(arg_2 + 0x46);
				break;
			case 2:
				local_10.field_21c.x += *(short const *)(arg_2 + 0x48);
				local_10.field_21c.y += *(short const *)(arg_2 + 0x4a);
				break;
			}
			function_22bcaf((s_widget_transform_block const *)(arg_2 + 0x4c), &local_10.field_21c, &local_10.field_214, &local_10.field_20c);
			local_10.field_21c.x -= g_4b9dd2;
			local_10.field_21c.y -= g_4b9dd0;
			local_10.field_0214.left = 0;
			local_10.field_0214.top = 0;
			local_10.field_0214.right = 1000;
			local_10.field_0214.bottom = 1000;
			
			function_13e9c0(local_10.field_0, &local_10.field_0214, &local_10.field_200, (short_rectangle2d *)&local_10.field_20c, 1.0f);
			long local_8 = (short)(*(dword const *)(local_10.field_0200 + 6) - *(dword const *)(local_10.field_0200 + 2));
			long local_9 = (short)(*(dword const *)(local_10.field_0200 + 4) - *(dword const *)local_10.field_0200);
			function_13eb60(g_4686cc);
			function_13ed50((color4f const *)g_4686d4);
			switch (*(word const *)(arg_2 + 0x2c))
			{
			case 0:
				local_10.field_0214.left = (short)local_10.field_21c.x;
				local_10.field_0214.top = (short)local_10.field_21c.y;
				local_10.field_0214.right = (short)(local_10.field_21c.x + local_8);
				local_10.field_0214.bottom = (short)(local_10.field_21c.y + local_9);
				break;
			case 1:
				local_10.field_0214.left = (short)(local_10.field_21c.x - (local_8 >> 1));
				local_10.field_0214.top = (short)local_10.field_21c.y;
				local_10.field_0214.right = (short)(local_10.field_21c.x + (local_8 - (local_8 >> 1)));
				local_10.field_0214.bottom = (short)(local_10.field_21c.y + local_9);
				break;
			case 2:
				local_10.field_0214.left = (short)(local_10.field_21c.x - local_8);
				local_10.field_0214.top = (short)local_10.field_21c.y;
				local_10.field_0214.right = (short)local_10.field_21c.x;
				local_10.field_0214.bottom = (short)(local_10.field_21c.y + local_9);
				break;
			}
			((c_1fa50 const *)local_10.field_0)->function_1fb10(&local_10.field_0214, 1.0f);
		}
	}
}
#pragma optimize("y", on)
