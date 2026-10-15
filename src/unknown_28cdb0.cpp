#include "unknown_11c920.h"
#include <xmmintrin.h>
#include "globals.h"
#include "animation_sampling.h"
// @flags /O2 /arch:SSE /Gr

/* animation channel decoding: each channel keeps a sorted run of frame
   numbers (bytes or words) with its keys beside it; the current frame is
   found by binary search and the surrounding keys are interpolated. */

struct s_translation_key
{
	real x, y, z;
};

struct s_scale_key
{
	real value;
};

struct s_animation_data
{
	byte unknown00[0x0c];
	long translation_indices;
	long scale_indices;
	long rotation_frames;
	long translation_frames;
	long scale_frames;
	long rotation_keys;
	long translation_keys;
	long scale_keys;
	byte unknown2c[4];
	dword rotation_indices[1];
};

struct s_animation_output
{
	real rotation[4];
	s_translation_key translation;
	s_scale_key scale;
};


/* normalized interpolation of two quantized (word) quaternions, weights t0
   and t1; uses the locals a, b, t0, t1 and result */
#define BLEND_ROTATION() \
	__asm mov ecx, a \
	__asm mov edx, b \
	__asm mov eax, result \
	__asm movq mm3, [ecx] \
	__asm punpcklwd mm1, mm3 \
	__asm punpckhwd mm2, mm3 \
	__asm psrad mm1, 0x10 \
	__asm psrad mm2, 0x10 \
	__asm cvtpi2ps xmm1, mm1 \
	__asm cvtpi2ps xmm2, mm2 \
	__asm movq mm3, [edx] \
	__asm punpcklwd mm1, mm3 \
	__asm punpckhwd mm2, mm3 \
	__asm psrad mm1, 0x10 \
	__asm psrad mm2, 0x10 \
	__asm movss xmm5, t0 \
	__asm movss xmm6, t1 \
	__asm shufps xmm5, xmm5, 0 \
	__asm shufps xmm6, xmm6, 0 \
	__asm cvtpi2ps xmm3, mm1 \
	__asm cvtpi2ps xmm4, mm2 \
	__asm movlhps xmm1, xmm2 \
	__asm movlhps xmm3, xmm4 \
	__asm mulps xmm1, xmm5 \
	__asm mulps xmm3, xmm6 \
	__asm addps xmm1, xmm3 \
	__asm emms \
	__asm movaps xmm0, xmm1 \
	__asm mulps xmm0, xmm1 \
	__asm movaps xmm3, xmm0 \
	__asm shufps xmm3, xmm3, 0x4e \
	__asm addps xmm0, xmm3 \
	__asm movaps xmm4, xmm0 \
	__asm shufps xmm4, xmm4, 0x11 \
	__asm addps xmm0, xmm4 \
	__asm rsqrtps xmm0, xmm0 \
	__asm mov eax, result \
	__asm mulps xmm1, xmm0 \
	__asm movaps [eax], xmm1

/* the same for a single quaternion; uses the locals a and result */
#define LOAD_ROTATION() \
	__asm mov ecx, a \
	__asm mov eax, result \
	__asm movq mm3, [ecx] \
	__asm punpcklwd mm1, mm3 \
	__asm punpckhwd mm2, mm3 \
	__asm psrad mm1, 0x10 \
	__asm psrad mm2, 0x10 \
	__asm cvtpi2ps xmm1, mm1 \
	__asm cvtpi2ps xmm2, mm2 \
	__asm emms \
	__asm movlhps xmm1, xmm2 \
	__asm movaps xmm0, xmm1 \
	__asm mulps xmm0, xmm1 \
	__asm movaps xmm3, xmm0 \
	__asm shufps xmm3, xmm3, 0x4e \
	__asm addps xmm0, xmm3 \
	__asm movaps xmm4, xmm0 \
	__asm shufps xmm4, xmm4, 0x11 \
	__asm addps xmm0, xmm4 \
	__asm rsqrtps xmm0, xmm0 \
	__asm mulps xmm1, xmm0 \
	__asm movaps [eax], xmm1

/* r = 1/x */
#define RECIPROCAL(r, x) \
	__asm rcpss xmm0, x \
	__asm movss r, xmm0

// @retail 0x28d170
char __stdcall function_28d170(
	long a,
	long b,
	long c,
	long d)
{
	return 0;
}

typedef char (__stdcall *t_function_28d170)(long, long, long, long);
t_function_28d170 g_function_28d170 = function_28d170;

// @retail 0x28cdb0
void function_28cdb0()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = data->rotation_indices[g_5044b4];
	dword start = index >> 12;
	long count = index & 0xfff;
	byte *frames = (byte *)data + (data->rotation_frames + start);
	short *keys = (short *)((byte *)data + data->rotation_keys) + start * 4;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	frames += low;
	dword frame = *frames;
	_mm_prefetch((char const *)(keys + low * 4), _MM_HINT_T0);
	if (g_sampling_settings.frame_index != frame)
	{
		s_animation_output *result = g_5044c0;
		real t0 = (real)(long)(frames[1] - g_sampling_settings.frame_index);
		real t1 = (real)(long)(g_sampling_settings.frame_index - frame);
		short *b = keys + low * 4 + 4;
		short *a = keys + low * 4;
		BLEND_ROTATION();
	}
	else
	{
		s_animation_output *result = g_5044c0;
		short *a = keys + low * 4;
		LOAD_ROTATION();
	}
}

// @retail 0x28cf40
void function_28cf40()
{
	long local_0 = g_5044b8;
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->translation_indices + local_0 * 4);
	long start = index >> 12;
	long count = index & 0xfff;
	byte *frames = (byte *)data + data->translation_frames + start;
	s_translation_key *keys = (s_translation_key *)((byte *)data + data->translation_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	frames += low;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_translation_key *key = keys + low;
	s_translation_key *translation = &g_5044c0->translation;
	*translation = *key;
	if (low + 1 < count)
	{
		dword frame = *frames;
		if (g_sampling_settings.frame_index != frame)
		{
			real local_1 = (real)(long)g_sampling_settings.frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			real t = reciprocal * local_1;
			s_translation_key b = key[1];
			translation->x += (b.x - translation->x) * t;
			translation->y += (b.y - translation->y) * t;
			translation->z += (b.z - translation->z) * t;
		}
	}
}

PRIVATE __forceinline void function_28d091(real arg_0, real arg_1, real arg_2, real arg_3, real *arg_4)
{
    *arg_4 = (arg_1 - arg_0) * (arg_2 * arg_3) + arg_0;
}

#pragma optimize("p", on)
// @retail 0x28d090
void function_28d090()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->scale_indices + g_5044bc * 4);
	long start = index >> 12;
	long count = index & 0xfff;
	byte *frames = (byte *)data + data->scale_frames + start;
	s_scale_key *keys = (s_scale_key *)((byte *)data + data->scale_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	s_animation_output *result = g_5044c0;
	frames += low;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_scale_key *key = keys + low;
	result->scale = *key;
	if (low + 1 < count)
	{
		dword frame = *frames;
		long local_3 = g_sampling_settings.frame_index;
		if (local_3 != frame)
		{
			real local_0 = (real)local_3 - (real)(long)frame;
			real denominator = (real)(long)(frames[1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			s_scale_key next = key[1];
			function_28d091(result->scale.value, next.value, reciprocal, local_0, &result->scale.value);
		}
	}
}
#pragma optimize("", on)

// @retail 0x28d180
void function_28d180()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = data->rotation_indices[g_5044b4];
	long start = index >> 12;
	word *frames = (word *)((byte *)data + data->rotation_frames) + start;
	long count = index & 0xfff;
	short *keys = (short *)((byte *)data + data->rotation_keys) + start * 4;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low * 4), _MM_HINT_T0);
	s_animation_output *result = g_5044c0;
	if (low + 1 < count)
	{
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real t0 = (real)(long)frames[low + 1] - frame;
		real t1 = frame - (real)(long)frames[low];
		short *b = keys + low * 4 + 4;
		short *a = keys + low * 4;
		BLEND_ROTATION();
	}
	else
	{
		short *a = keys + low * 4;
		LOAD_ROTATION();
	}
}

// @retail 0x28d320
void function_28d320()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->translation_indices + g_5044b8 * 4);
	dword start = index >> 12;
	long count = index & 0xfff;
	word *frames = (word *)((byte *)data + data->translation_frames) + start;
	s_translation_key *keys = (s_translation_key *)((byte *)data + data->translation_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	s_translation_key *translation = &g_5044c0->translation;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_translation_key *key = keys + low;
	*translation = *key;
	if (low + 1 < count)
	{
		dword cur = frames[low];
		dword following = frames[low + 1];
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		frame -= (real)(long)cur;
		real denominator = (real)(long)(following - cur);
		real reciprocal;
		RECIPROCAL(reciprocal, denominator);
		real t = reciprocal * frame;
		s_translation_key b = key[1];
		translation->x += (b.x - translation->x) * t;
		translation->y += (b.y - translation->y) * t;
		translation->z += (b.z - translation->z) * t;
	}
}

// @retail 0x28d470
void function_28d470()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->scale_indices + g_5044bc * 4);
	dword start = index >> 12;
	long count = index & 0xfff;
	word *frames = (word *)((byte *)data + data->scale_frames) + start;
	s_scale_key *keys = (s_scale_key *)((byte *)data + data->scale_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_scale_key *key = keys + low;
	s_animation_output *result = g_5044c0;
	result->scale = *key;
	if (low + 1 < count)
	{
		dword cur = frames[low];
		dword following = frames[low + 1];
		real frame = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real local_0 = frame - (real)(long)cur;
		real denominator = (real)(long)(following - cur);
		real reciprocal;
		RECIPROCAL(reciprocal, denominator);
		s_scale_key next = key[1];
		function_28d091(result->scale.value, next.value, reciprocal, local_0, &result->scale.value);
	}
}

// @retail 0x28d560
void function_28d560()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = data->rotation_indices[g_5044b4];
	dword start = index >> 12;
	long count = index & 0xfff;
	word *frames = (word *)((byte *)data + data->rotation_frames) + start;
	short *keys = (short *)((byte *)data + data->rotation_keys) + start * 4;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	dword frame = frames[low];
	_mm_prefetch((char const *)(keys + low * 4), _MM_HINT_T0);
	if (g_sampling_settings.frame_index != frame)
	{
		s_animation_output *result = g_5044c0;
		real t0 = (real)(long)(frames[low + 1] - g_sampling_settings.frame_index);
		real t1 = (real)(long)(g_sampling_settings.frame_index - frame);
		short *b = keys + low * 4 + 4;
		short *a = keys + low * 4;
		BLEND_ROTATION();
	}
	else
	{
		s_animation_output *result = g_5044c0;
		short *a = keys + low * 4;
		LOAD_ROTATION();
	}
}

// @retail 0x28d6f0
void function_28d6f0()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->translation_indices + g_5044b8 * 4);
	dword start = index >> 12;
	long count = index & 0xfff;
	word *frames = (word *)((byte *)data + data->translation_frames) + start;
	s_translation_key *keys = (s_translation_key *)((byte *)data + data->translation_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	s_translation_key *translation = &g_5044c0->translation;
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_translation_key *key = keys + low;
	*translation = *key;
	if (low + 1 < count)
	{
		dword frame = frames[low];
		if (g_sampling_settings.frame_index != frame)
		{
			real t = (real)(long)g_sampling_settings.frame_index - (real)(long)frame;
			real denominator = (real)(long)(frames[low + 1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			t = reciprocal * t;
			s_translation_key b = key[1];
			translation->x += (b.x - translation->x) * t;
			translation->y += (b.y - translation->y) * t;
			translation->z += (b.z - translation->z) * t;
		}
	}
}

#pragma optimize("p", on)
// @retail 0x28d840
void function_28d840()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword index = *(dword *)((byte *)data + data->scale_indices + g_5044bc * 4);
	dword start = index >> 12;
	long count = index & 0xfff;
	word *frames = (word *)((byte *)data + data->scale_frames) + start;
	s_scale_key *keys = (s_scale_key *)((byte *)data + data->scale_keys) + start;
	long low = 0;
	long high = count;
	while (high > low + 1)
	{
		long middle = (high + low) >> 1;
		if (frames[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}
	_mm_prefetch((char const *)(keys + low), _MM_HINT_T0);
	s_scale_key *key = keys + low;
	s_scale_key &output = g_5044c0->scale;
	output = *key;
	if (low + 1 < count)
	{
		dword frame = frames[low];
		long local_3 = g_sampling_settings.frame_index;
		if (local_3 != frame)
		{
			real local_0 = (real)local_3 - (real)(long)frame;
			real denominator = (real)(long)(frames[low + 1] - frame);
			real reciprocal;
			RECIPROCAL(reciprocal, denominator);
			s_scale_key next = key[1];
			function_28d091(output.value, next.value, reciprocal, local_0, &output.value);
		}
	}
}
#pragma optimize("", on)
