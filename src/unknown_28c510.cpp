#include "unknown_11c920.h"
#include <xmmintrin.h>
#include "globals.h"
#include "unknown_xd56787.h"

// @flags /O2 /Gr /arch:SSE

struct s_frame_info
{
	dword count : 12;
	dword start : 20;
};

struct s_animation_data
{
	byte unknown00[0xc];
	long vector_offset;
	long scale_offset;
	long rotation_stride;
	long vector_stride;
	long scale_stride;
	long unknown20;
	long unknown24;
	long unknown28;
	long unknown2c;
	dword rotation_frame_info[1];
};


// @retail 0x28c510
void function_28c510()
{
	g_5044c0->scale = *(real *)((byte *)g_sampling_settings.field_30 + g_sampling_settings.field_30->scale_offset + g_5044bc * 4);
}

// @retail 0x28c530
void function_28c530()
{
	quaternionf *quaternions = (quaternionf *)((byte *)g_sampling_settings.field_30 + g_sampling_settings.field_30->rotation_stride * g_5044b4 + 0x20);
	quaternionf *result = &g_5044c0->rotation;
	real t = g_sampling_settings.frame_fraction;
	quaternionf *b = quaternions + g_sampling_settings.next_frame_index;
	quaternionf *a = quaternions + g_sampling_settings.frame_index;

	__asm
	{
		mov eax, a
		mov ecx, b
		movss xmm3, t
		movaps xmm1, [eax]
		movaps xmm2, [ecx]
		shufps xmm3, xmm3, 0
		subps xmm2, xmm1
		mulps xmm2, xmm3
		addps xmm2, xmm1
		movaps xmm0, xmm2
		mulps xmm0, xmm2
		movaps xmm7, xmm0
		shufps xmm7, xmm7, 0x4e
		addps xmm0, xmm7
		movaps xmm6, xmm0
		shufps xmm6, xmm6, 0x11
		addps xmm0, xmm6
		rsqrtps xmm0, xmm0
		mov edx, result
		mulps xmm2, xmm0
		movaps [edx], xmm2
	}
}

PRIVATE __forceinline vector3f *function_28c5d1(s_animation_data *arg_0, long arg_1)
{
	long local_0 = arg_0->vector_stride;
	long local_1 = *(volatile long *)&arg_0->vector_offset;
	return (vector3f *)((byte *)arg_0 + (local_0 * arg_1 + local_1));
}

PRIVATE __forceinline void function_28c5d2(vector3f const *arg_0, vector3f const *arg_1, real arg_2, vector3f *arg_3)
{
	arg_3->i = (arg_1->i - arg_0->i) * arg_2 + arg_0->i;
	arg_3->j = (arg_1->j - arg_0->j) * arg_2 + arg_0->j;
	arg_3->k = (arg_1->k - arg_0->k) * arg_2 + arg_0->k;
}

PRIVATE __forceinline vector3f *function_28c5d3(long arg_0, s_animation_data *const &arg_1)
{
	long local_0 = arg_1->vector_stride;
	long local_1 = *(volatile long *)&arg_1->vector_offset;
	return (vector3f *)(((byte *)arg_1 + local_0 * arg_0) + local_1);
}

// @retail 0x28c5d0
void function_28c5d0()
{
	static vector3f vector_a;
	static vector3f vector_b;
	s_animation_data *local_0 = g_sampling_settings.field_30;
	vector3f *vectors = function_28c5d3(g_5044b8, local_0);
	real t = g_sampling_settings.frame_fraction;

	vector_a = vectors[g_sampling_settings.frame_index];
	vector_b = vectors[g_sampling_settings.next_frame_index];
	function_28c5d2(&vector_a, &vector_b, t, &g_5044c0->vector);
}

// @retail 0x28c6b0
void function_28c6b0()
{
	dword *scales = (dword *)((byte *)g_sampling_settings.field_30 + g_sampling_settings.field_30->scale_stride * g_5044bc + g_sampling_settings.field_30->scale_offset);
	dword bits_a = scales[g_sampling_settings.frame_index];
	dword bits_b = scales[g_sampling_settings.next_frame_index];
	real a = *(real *)&bits_a;
	real b = *(real *)&bits_b;

	g_5044c0->scale = (b - a) * g_sampling_settings.frame_fraction + a;
}

// @retail 0x28c710
void function_28c710()
{
	quaternionf *quaternions = (quaternionf *)((byte *)g_sampling_settings.field_30 + g_sampling_settings.field_30->rotation_stride * g_5044b4);
	quaternionf *source = quaternions + (g_sampling_settings.frame_index + 2);
	quaternionf *destination = &g_5044c0->rotation;

	*destination = *source;
}

// @retail 0x28c750
void function_28c750()
{
	s_animation_data *data = g_sampling_settings.field_30;
	vector3f *source = (vector3f *)((byte *)data + (data->vector_stride * g_5044b8 + g_sampling_settings.frame_index * 12 + data->vector_offset));
	vector3f *destination = &g_5044c0->vector;

	*destination = *source;
}

// @retail 0x28c790
void function_28c790()
{
	quaternionf *quaternions = (quaternionf *)((byte *)g_sampling_settings.field_30 + g_sampling_settings.field_30->rotation_stride * g_5044b4 + 0x20);
	real inverse_t = 1.0f - g_sampling_settings.frame_fraction;
	real t = g_sampling_settings.frame_fraction;
	quaternionf *result = &g_5044c0->rotation;
	short *b = (short *)quaternions + g_sampling_settings.next_frame_index * 4;
	short *a = (short *)quaternions + g_sampling_settings.frame_index * 4;

	__asm
	{
		mov ecx, a
		mov edx, b
		mov eax, result
		movq mm3, qword ptr [ecx]
		punpcklwd mm1, mm3
		punpckhwd mm2, mm3
		psrad mm1, 0x10
		psrad mm2, 0x10
		cvtpi2ps xmm1, mm1
		cvtpi2ps xmm2, mm2
		movq mm3, qword ptr [edx]
		punpcklwd mm1, mm3
		punpckhwd mm2, mm3
		psrad mm1, 0x10
		psrad mm2, 0x10
		movss xmm5, inverse_t
		movss xmm6, t
		shufps xmm5, xmm5, 0
		shufps xmm6, xmm6, 0
		cvtpi2ps xmm3, mm1
		cvtpi2ps xmm4, mm2
		movlhps xmm1, xmm2
		movlhps xmm3, xmm4
		mulps xmm1, xmm5
		mulps xmm3, xmm6
		addps xmm1, xmm3
		emms
		movaps xmm0, xmm1
		mulps xmm0, xmm1
		movaps xmm3, xmm0
		shufps xmm3, xmm3, 0x4e
		addps xmm0, xmm3
		movaps xmm4, xmm0
		shufps xmm4, xmm4, 0x11
		addps xmm0, xmm4
		rsqrtps xmm0, xmm0
		mov eax, result
		mulps xmm1, xmm0
		movaps [eax], xmm1
	}
}

// @retail 0x28c880
void function_28c880()
{
	static vector3f vector_a;
	static vector3f vector_b;
	s_animation_data *local_0 = g_sampling_settings.field_30;
	vector3f *vectors = function_28c5d3(g_5044b8, local_0);
	real t = g_sampling_settings.frame_fraction;

	vector_a = vectors[g_sampling_settings.frame_index];
	vector_b = vectors[g_sampling_settings.next_frame_index];
	function_28c5d2(&vector_a, &vector_b, t, &g_5044c0->vector);
}

// @retail 0x28c960
void function_28c960()
{
	s_animation_data *local_0 = g_sampling_settings.field_30;
	long local_1 = local_0->rotation_stride * g_5044b4;
	quaternionf *result = &g_5044c0->rotation;
	short *a = (short *)((byte *)local_0 + local_1 + 0x20) + g_sampling_settings.frame_index * 4;

	__asm
	{
		mov ecx, a
		mov eax, result
		movq mm3, qword ptr [ecx]
		punpcklwd mm1, mm3
		punpckhwd mm2, mm3
		psrad mm1, 0x10
		psrad mm2, 0x10
		cvtpi2ps xmm1, mm1
		cvtpi2ps xmm2, mm2
		emms
		movlhps xmm1, xmm2
		movaps xmm0, xmm1
		mulps xmm0, xmm1
		movaps xmm3, xmm0
		shufps xmm3, xmm3, 0x4e
		addps xmm0, xmm3
		movaps xmm4, xmm0
		shufps xmm4, xmm4, 0x11
		addps xmm0, xmm4
		rsqrtps xmm0, xmm0
		mulps xmm1, xmm0
		movaps [eax], xmm1
	}
}

// @retail 0x28c9e0
void function_28c9e0()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword frame_info = data->rotation_frame_info[g_5044b4];
	long start = frame_info >> 12;
	long count = frame_info & 0xfff;
	byte *keys = (byte *)data + data->rotation_stride + start;
	short *quaternions = (short *)((byte *)data + data->unknown20 + start * 8);
	long low = 0;
	long high = count;
	quaternionf *result;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)keys[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(quaternions + low * 4), _MM_HINT_T0);
	result = &g_5044c0->rotation;
	keys += low;

	if (low + 1 < count)
	{
		real x = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		real weight_a = keys[1] - x;
		real weight_b = x - keys[0];
		short *b = quaternions + (low + 1) * 4;
		short *a = quaternions + low * 4;

		__asm
		{
			mov ecx, a
			mov edx, b
			mov eax, result
			movq mm3, qword ptr [ecx]
			punpcklwd mm1, mm3
			punpckhwd mm2, mm3
			psrad mm1, 0x10
			psrad mm2, 0x10
			cvtpi2ps xmm1, mm1
			cvtpi2ps xmm2, mm2
			movq mm3, qword ptr [edx]
			punpcklwd mm1, mm3
			punpckhwd mm2, mm3
			psrad mm1, 0x10
			psrad mm2, 0x10
			movss xmm5, weight_a
			movss xmm6, weight_b
			shufps xmm5, xmm5, 0
			shufps xmm6, xmm6, 0
			cvtpi2ps xmm3, mm1
			cvtpi2ps xmm4, mm2
			movlhps xmm1, xmm2
			movlhps xmm3, xmm4
			mulps xmm1, xmm5
			mulps xmm3, xmm6
			addps xmm1, xmm3
			emms
			movaps xmm0, xmm1
			mulps xmm0, xmm1
			movaps xmm3, xmm0
			shufps xmm3, xmm3, 0x4e
			addps xmm0, xmm3
			movaps xmm4, xmm0
			shufps xmm4, xmm4, 0x11
			addps xmm0, xmm4
			rsqrtps xmm0, xmm0
			mov eax, result
			mulps xmm1, xmm0
			movaps [eax], xmm1
		}
	}
	else
	{
		short *a = quaternions + low * 4;
		__asm
		{
			mov ecx, a
			mov eax, result
			movq mm3, qword ptr [ecx]
			punpcklwd mm1, mm3
			punpckhwd mm2, mm3
			psrad mm1, 0x10
			psrad mm2, 0x10
			cvtpi2ps xmm1, mm1
			cvtpi2ps xmm2, mm2
			emms
			movlhps xmm1, xmm2
			movaps xmm0, xmm1
			mulps xmm0, xmm1
			movaps xmm3, xmm0
			shufps xmm3, xmm3, 0x4e
			addps xmm0, xmm3
			movaps xmm4, xmm0
			shufps xmm4, xmm4, 0x11
			addps xmm0, xmm4
			rsqrtps xmm0, xmm0
			mulps xmm1, xmm0
			movaps [eax], xmm1
		}
	}
}

PRIVATE __forceinline void function_28cb72(vector3f const *arg_0, real arg_1, vector3f *arg_2)
{
    vector3f local_0;
    struct words { dword i, j, k; };
    *(words *)&local_0 = *(words const *)arg_0;
    arg_2->i = (local_0.i - arg_2->i) * arg_1 + arg_2->i;
    arg_2->j = (local_0.j - arg_2->j) * arg_1 + arg_2->j;
    arg_2->k = (local_0.k - arg_2->k) * arg_1 + arg_2->k;
}

// @retail 0x28cb70
void function_28cb70()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword frame_info = *(dword *)((byte *)data + (data->vector_offset + g_5044b8 * 4));
	long start = frame_info >> 12;
	long count = frame_info & 0xfff;
	byte *keys = (byte *)data + data->vector_stride + start;
	vector3f *vectors = (vector3f *)((byte *)data + data->unknown24 + start * 12);
	long low = 0;
	long high = count;
	vector3f *vector;
	vector3f *destination;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)keys[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(vectors + low), _MM_HINT_T0);
	vector = vectors + low;
	destination = &g_5044c0->vector;
	*destination = *vector;
	keys += low;

	if (low + 1 < count)
	{
		long local_0 = keys[0];
		long local_1 = keys[1];
		real x = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		x -= (real)local_0;
		real difference = (real)(local_1 - local_0);
		real reciprocal;
		real weight;

		__asm
		{
			rcpss xmm0, difference
			movss reciprocal, xmm0
		}

		weight = reciprocal * x;
		function_28cb72(vector + 1, weight, destination);
	}
}

PRIVATE __forceinline void function_28cccf(real arg_0, real arg_1, real arg_2, real arg_3, real *arg_4)
{
    *arg_4 = (arg_1 - arg_0) * (arg_2 * arg_3) + arg_0;
}

// @retail 0x28ccc0
void function_28ccc0()
{
	s_animation_data *data = g_sampling_settings.field_30;
	dword frame_info = *(dword *)((byte *)data + (data->scale_offset + g_5044bc * 4));
	long start = frame_info >> 12;
	long count = frame_info & 0xfff;
	byte *keys = (byte *)data + data->scale_stride + start;
	dword *scales = (dword *)((byte *)data + data->unknown28 + start * 4);
	long low = 0;
	long high = count;
	dword *scale;
	s_animation_output *result;

	while (high > low + 1)
	{
		long middle = (low + high) >> 1;

		if ((dword)keys[middle] <= (dword)g_sampling_settings.frame_index)
		{
			low = middle;
		}
		else
		{
			high = middle;
		}
	}

	_mm_prefetch((char *)(scales + low), _MM_HINT_T0);
	scale = scales + low;
	result = g_5044c0;
	*(dword *)&result->scale = *scale;
	keys += low;

	if (low + 1 < count)
	{
		long local_0 = keys[0];
		long local_1 = keys[1];
		real x = (real)(long)g_sampling_settings.frame_index + g_sampling_settings.frame_fraction;
		x -= (real)local_0;
		real difference = (real)(local_1 - local_0);
		real reciprocal;
		real weight;

		__asm
		{
			rcpss xmm0, difference
			movss reciprocal, xmm0
		}

		weight = reciprocal * x;
		dword bits_next = scale[1];
		real next = *(real *)&bits_next;

		function_28cccf(result->scale, next, reciprocal, x, &result->scale);
	}
}
