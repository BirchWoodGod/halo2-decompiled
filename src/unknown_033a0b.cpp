// @flags /O1 /arch:SSE /Gr
#include "view_context_storage.h"
/* UNKNOWN_033A0B.CPP: vector getters */

/* Not functions of their own: each is a case body of the switch that starts
   function 0x33980, at the address its "case at" line gives. They await
   folding into that function. */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_33a0b_view
{
	byte unknown00[0x10];
	vector3f vector10;
	byte unknown1c[0x14];
	vector3f vector30;
	vector3f vector3c;
	vector3f vector48;
	vector3f vector54;
};

struct s_33a0b_default
{
	dword unknown0;
	vector3f vector;
};

struct s_dwords3 { dword a, b, c; };

inline PRIVATE void copy_vector(vector3f *dest, vector3f const *src)
{
	s_dwords3 *d = (s_dwords3 *)dest;
	s_dwords3 const *s = (s_dwords3 const *)src;
	d->a = s->a;
	d->b = s->b;
	d->c = s->c;
}

vector3f g_4859dc;
vector3f g_4859ec;
s_33a0b_view *g_485a58;
s_33a0b_default *g_4686d4;
real g_485a6c;
real g_485a70;

inline PRIVATE real pin_unit(real value)
{
	if (value < 0.0f)
	{
		value = 0.0f;
	}
	else if (value > 1.0f)
	{
		value = 1.0f;
	}
	return value;
}

// case at 0x33a0b
void function_033a0b(vector3f *out)
{
	copy_vector(out, &g_4859dc);
}

// case at 0x33a25
void function_033a25(vector3f *out)
{
	copy_vector(out, &g_4859ec);
}

// case at 0x33a3f
void function_033a3f(vector3f *out)
{
	vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector3c;
	copy_vector(out, vector);
}

// case at 0x33a66
void function_033a66(vector3f *out)
{
	vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector30;
	copy_vector(out, vector);
}

// case at 0x33ac5
void function_033ac5(vector3f *out)
{
	copy_vector(out, &g_485a58->vector48);
}

// case at 0x33ada
void function_033ada(vector3f *out)
{
	vector3f *vector = !g_485a58 ? &g_4686d4->vector : &g_485a58->vector30;
	copy_vector(out, vector);
	out->i = pin_unit(out->i * g_485a6c);
	out->j = pin_unit(out->j * g_485a6c);
	out->k = pin_unit(out->k * g_485a6c);
}

// case at 0x33cdd
void function_033cdd(vector3f *out)
{
	copy_vector(out, &g_485768);
}

// case at 0x33cf7
void function_033cf7(vector3f *out)
{
	out->i = g_485768.i * g_485774;
	out->j = g_485768.j * g_485774;
	out->k = g_485768.k * g_485774;
}

// case at 0x33d36
void function_033d36(vector3f *out)
{
	copy_vector(out, &g_485780);
}

color3f *unpack_color3f(dword pixel, color3f *color);
real function_33670(long selector);

struct s_4e6950
{
	byte unknown00[0x70];
	real user_values[4];
};
extern s_4e6950 g_4e6950;

// @retail 0x33980
void function_33980(long selector, vector3f *out)
{
	switch (selector)
	{
	case 0:
		out->i = g_485618.forward.i;
		out->j = g_485618.forward.j;
		out->k = g_485618.forward.k;
		return;
	case 3:
	case 4:
	case 21:
	case 22:
	{
		long index;
		switch (selector)
		{
		case 3: index = 0; break;
		case 4: index = 1; break;
		case 21: index = 2; break;
		case 22: index = 3; break;
		default: __assume(0);
		}
		byte *view = (byte *)g_485a58;
		if (view && index >= 0 && index < view[0x74])
			unpack_color3f(((dword *)(view + 0x64))[index], (color3f *)out);
		else copy_vector(out, &(*(vector3f *)g_468710));
		return;
	}
	case 5: copy_vector(out, &(g_4859dc)); return;
	case 6: copy_vector(out, &(g_4859ec)); return;
	case 14: copy_vector(out, &(g_485a58 ? g_485a58->vector3c : g_4686d4->vector)); return;
	case 15: copy_vector(out, &(g_485a58 ? g_485a58->vector30 : g_4686d4->vector)); return;
	case 16: copy_vector(out, &(g_485a58 ? g_485a58->vector54 : g_4686d4->vector)); return;
	case 17: copy_vector(out, &(g_485a58 ? g_485a58->vector48 : g_4686d4->vector)); return;
	case 18: copy_vector(out, &(g_485768)); return;
	case 19: copy_vector(out, &(g_4857ec)); return;
	case 23:
		copy_vector(out, &(g_485a58 ? g_485a58->vector30 : g_4686d4->vector));
		out->i = pin_unit(out->i * g_485a6c);
		out->j = pin_unit(out->j * g_485a6c);
		out->k = pin_unit(out->k * g_485a6c);
		return;
	case 24:
		copy_vector(out, &(g_485a58 ? g_485a58->vector10 : g_4686d4->vector));
		out->i = pin_unit((g_485a70 + 1.0f) * out->i);
		out->j = pin_unit((g_485a70 + 1.0f) * out->j);
		out->k = pin_unit((g_485a70 + 1.0f) * out->k);
		return;
	case 25: copy_vector(out, &(g_485768)); return;
	case 26:
		out->i = g_485768.i * g_485774;
		out->j = g_485768.j * g_485774;
		out->k = g_485768.k * g_485774;
		return;
	case 27: copy_vector(out, &(g_485780)); return;
	case 28:
		out->i = g_485780.i * g_48578c;
		out->j = g_485780.j * g_48578c;
		out->k = g_485780.k * g_48578c;
		return;
	case 29: copy_vector(out, &(g_485798)); return;
	case 30:
		out->i = g_485798.i * g_4857a4;
		out->j = g_485798.j * g_4857a4;
		out->k = g_485798.k * g_4857a4;
		return;
	case 31: copy_vector(out, &(g_4857ec)); return;
	case 32:
		out->i = g_4857ec.i * g_4857f8;
		out->j = g_4857ec.j * g_4857f8;
		out->k = g_4857ec.k * g_4857f8;
		return;
	case 33: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x00))); return;
	case 34: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x0c))); return;
	case 35:
		copy_vector(out, &(g_485a58 ? g_485a58->vector30 : g_4686d4->vector));
		out->i = pin_unit(out->i * g_485a6c * 0.5f);
		out->j = pin_unit(out->j * g_485a6c * 0.5f);
		out->k = pin_unit(out->k * g_485a6c * 0.5f);
		return;
	case 36: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x40))); return;
	case 37: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x4c))); return;
	case 38: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x58))); return;
	case 39: copy_vector(out, &(*(vector3f *)(g_4e6950.unknown00 + 0x64))); return;
	case 40:
	{
		real value = function_33670(*(long *)g_485a58);
		out->i = value;
		out->j = value;
		out->k = value;
		return;
	}
	case 41:
		if (g_4b9f5c != NONE && g_4b9f84 > g_4b9f80)
		{
			real inverse = 1.0f / (g_4b9f84 - g_4b9f80);
			out->i = 0.0f - g_4b9dac.i * inverse;
			out->j = 0.0f - g_4b9dac.j * inverse;
			out->k = 0.0f - g_4b9dac.k * inverse;
		}
		else copy_vector(out, &(g_4686d4->vector));
		return;
	default: __assume(0);
	}
}
