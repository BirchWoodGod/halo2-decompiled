// @flags /O2 /Op /arch:SSE /Gr
#include "render_source_storage.h"
/* UNKNOWN_0494B0.CPP: rasterizer render state helpers */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include <xtl.h>
#include "globals.h"

PRIVATE bool test_bit(dword flags, long bit)
{
	return (flags >> bit) & 1;
}

dword g_4b8344;
dword g_4b8340;
dword g_4b8464;
dword g_4b8450;
dword g_4b846c;
dword g_4b8448;
dword g_4b8308;
dword g_5093b4;
byte g_47fe84;
dword g_4b8348;
long g_4c1a14;
byte g_509416;
bool g_4aeb48;
extern long g_4858b8;
void function_14bc0(short index, short element, bool use_depth);

// @retail 0x35aa0
void function_35aa0(void)
{
    g_4c1a14 = 0;
    g_509416 = 0;
    function_14bc0((short)g_4858b8, 0, true);
    if (g_4aeb48)
    {
        __asm wbinvd
        g_4aeb48 = false;
    }
    g_4b8348 = 1;
    D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE, 1);
}

dword g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324;
dword const g_4506b8[12] = {32774,32774,32774,32774,32779,32775,32776,32774,32774,32774,0xffffffff,0xffffffff};
dword const g_4506e8[12] = {771,0,768,1,1,1,1,771,32770,32769,0xffffffff,0xffffffff};
dword const g_450718[12] = {770,774,774,1,1,1,1,1,32769,32770,0xffffffff,0xffffffff};

// @retail 0x142f0
void function_142f0(short mode)
{
    dword enabled = mode != 10;
    g_4b82e8 = enabled;
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, enabled);
    if (mode != 10)
    {
        dword source = g_450718[mode];
        g_4b82f4 = source;
        D3DDevice_SetRenderState(D3DRS_SRCBLEND, source);
        dword destination = g_4506e8[mode];
        g_4b82f8 = destination;
        D3DDevice_SetRenderState(D3DRS_DESTBLEND, destination);
        dword operation = g_4506b8[mode];
        g_4b8324 = operation;
        D3DDevice_SetRenderState(D3DRS_BLENDOP, operation);
    }
}

// @retail 0x48010
void function_48010(real const *color)
{
    D3DDevice_SetVertexData4f(9, color[1], color[2], color[3], color[0]);
}


dword g_4b8334;
dword g_4b8330;


real g_4c9798[36];
byte g_4b5144;
dword g_4b50c4;
dword g_4b50d8;
dword g_4b5130;
dword g_4b5134;

PRIVATE real normalize(real value, real lower, real upper)
{
	return (value - lower) / (upper - lower);
}

// @retail 0x494b0
void function_0494b0(real const *bounds, real const *window)
{
	real width = bounds[1] - bounds[0];
	real left = 1.0f - (window[0] - bounds[0]) / width;
	real right = 1.0f - (window[1] - bounds[0]) / width;
	real height = bounds[3] - bounds[2];
	real top = (window[2] - bounds[2]) / height;
	real bottom = (window[3] - bounds[2]) / height;
	real scale_x = 1.0f / (right - left);
	real scale_y = 1.0f / (bottom - top);

	g_4c9798[8] = 512.0f;
	g_4c9798[9] = 512.0f;
	g_4c9798[32] = scale_x;
	g_4c9798[33] = scale_y;
	g_4b50c4 = g_4b50d8 = g_4b5144 ? 0x1f1ff1ff : 0;
	g_4b5130 = g_4b5134 = 0x200;
	g_4c9798[10] = 16777215.0f;
	g_4c9798[11] = 1.0f;
	g_4c9798[34] = scale_x * left * -512.0f;
	g_4c9798[35] = scale_y * top * -512.0f;
	g_4ba014 |= 0x48;
	D3DDevice_SetVertexShaderConstantFast(-56, g_4c9798, 9);
}

// @retail 0x495e0
void function_0495e0(void)
{
	dword value = test_bit(g_4ba014, 4) ? 16 : 1;
	g_4b8344 = value;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, value);
	value = g_47fe84 ? 2 : 0;
	g_4b8464 = value;
	D3DDevice_SetRenderState(D3DRS_MULTISAMPLEMODE, value);
	g_4b8340 = 1;
	D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 1);
	real z_offset = 550.0f;
	g_4b8334 = *(dword *)&z_offset;
	D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, *(dword *)&z_offset);
	real z_slope = 2.0f;
	g_4b8330 = *(dword *)&z_slope;
	D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, *(dword *)&z_slope);
}

// @retail 0x496a0
void function_0496a0(void)
{
	g_4b8344 = 0;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
	g_4b8340 = 0;
	D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 0);
	g_4b8464 = 0;
	D3DDevice_SetRenderState(D3DRS_MULTISAMPLEMODE, 0);
	g_4b8450 = 0;
	D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
}

// @retail 0x496f0
void function_0496f0(void)
{
	g_4b846c = 0x204;
	D3DDevice_SetRenderState(D3DRS_SHADOWFUNC, 0x204);
	g_4b8344 = 0x10;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0x10);
	g_4b8448 = g_5093b4;
	D3DDevice_SetRenderState(D3DRS_CULLMODE, g_5093b4);
}

// @retail 0x49740
void function_049740(void)
{
	g_4b846c = 0x200;
	D3DDevice_SetRenderState(D3DRS_SHADOWFUNC, 0x200);
	g_4b8344 = 0;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
	g_4b8308 = 0x1010101;
	D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
real g_485b28[7];

// @retail 0x1beb0
void function_1beb0(void)
{
    real height = (real)(g_4b9dd4 - g_4b9dd0);
    g_485b28[0] = 1.0f / height;
    g_485b28[1] = 0.0f / height;
    g_485b28[2] = 0.0f / height;
    g_485b28[3] = 2.0f / height;
    g_485b28[4] = 4.0f / height;
    g_485b28[5] = 0.0f / height;
    g_485b28[6] = 1.0f / height;
    if (g_510c50 && ((byte *)g_510c50)[5])
        g_485b28[6] *= 2.0f;
}

// @retail 0x47ea0
void function_47ea0(short stage, byte flags)
{
	byte const *flags_reference = &flags;
	if (*flags_reference & 1)
	{
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
	}
	else
	{
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}
	if (*flags_reference & 2)
	{
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_POINT);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_POINT);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	}
	else
	{
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
	D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
}

extern byte *g_485a80;
bool function_14560(long tag, short stage, short fallback, short fallback_index, short index);
bool function_14480(long tag, short stage, short index);

// @retail 0x47fd0
bool function_47fd0(long tag, short stage, short fallback, byte flags)
{
    bool result;
    if (tag != NONE)
        result = function_14560(tag, stage, fallback, 0, 1);
    else
        result = function_14480(*(long *)(g_485a80 + 0x34), stage, fallback);
    if (!result)
        goto failure;
    return result;
failure:
    function_47ea0(stage, flags);
    return result;
}


#include <math.h>
#include "texture_stage_storage.h"
void function_1cf50(void);
void function_01ddd0(long index, dword width, dword height);
long function_48e70(real value);
real g_485a34, g_485a40;
byte g_485a47;
real g_485a0c[8];

// @retail 0x48ee0
void function_48ee0(byte const *state, byte const *matrix, byte const *tag)
{
    real aspect = (real)sqrt(*(real const *)(state + 8));
    real distance = *(real const *)(state + 0x64);
    real offset = *(short const *)(tag + 0x8e) == 1 ? *(real const *)(state + 0x28) : 0.0f;
    real inverse = distance != 0.0f ? 1.0f / distance : -3.4028234663852886e38f;
    vector3f const &direction = *(vector3f const *)(matrix + 0x28);
    g_4c9798[0] = direction.i * inverse;
    g_4c9798[1] = direction.j * inverse;
    g_4c9798[2] = direction.k * inverse;
    g_4c9798[3] = 0.0f - inverse * offset;
    g_4c9798[4] = 0.0f - direction.i;
    g_4c9798[5] = 0.0f - direction.j;
    g_4c9798[6] = 0.0f - direction.k;
    g_4c9798[7] = 0.0f;
    long volatile mode = !(g_4c1a80 & 0x400) ? 2 : g_485a40 > 0.0f && *(short const *)(tag + 0x14) == 0 ? 1 : 0;
    real volatile initial_extent = *(real const *)(state + 4);
    double extent = initial_extent;
    if (*(real const *)(state + 0xc) > 0.0f)
        extent += 2.0 * tan((double)*(real const *)(state + 0xc) * 0.5f) * distance;
    long max_width = (g_4ba014 & 0x10) && !(g_4ba014 & 0x20) ? 128 : 512;
    long max_height = (g_4ba014 & 0x10) && !(g_4ba014 & 0x20) ? 128 : 512;
    real volatile maximum = (real)max_width;
    double desired = (sqrt(extent * extent * *(real const *)(state + 8)) * 0.3333333432674408f + *(real const *)(tag + 0x10) * (double)0.125f) * (maximum - (double)128.0f) * g_485a34 + 128.0f;
    real size = (real)desired;
    if (desired < 128.0f) size = 128.0f;
    else if (size > maximum) size = maximum;
    long width = (function_48e70(size / aspect) + 63) & ~63;
    long height = (function_48e70(size * aspect) + 63) & ~63;
    if (width * height > max_width * max_height)
    {
        if (width > height) width = (max_width * max_height / height) & ~63;
        else height = (max_width * max_height / width) & ~63;
    }
    g_485a47 = (byte)mode;
    g_51f3c8[1][0] = 0;
    g_51f3c8[1][1] = 0;
    g_51f3c8[1][2] = 0;
    g_51f3c8[1][3] = 0;
    function_1cf50();
    if ((g_4ba014 & 0x10) && !(g_4ba014 & 0x20))
    {
        g_4c9798[8] = maximum;
        g_4c9798[9] = (real)max_height;
    }
    else
    {
        function_01ddd0(9, width, height);
        g_4c9798[8] = (real)width;
        g_4c9798[9] = (real)height;
    }
    g_4c9798[10] = 16777215.0f;
    g_4c9798[11] = 1.0f;
    g_4c9798[32] = g_4c9798[33] = 1.0f;
    g_4c9798[34] = g_4c9798[35] = 0.0f;
    real border = mode > 0 ? (real)width * 0.75f / maximum : 0.0f;
    g_4c9798[12] = border + 0.5f;
    g_4c9798[13] = 0.5f - border;
    g_4c9798[15] = 0.0f;
    point3f const &origin = *(point3f const *)matrix;
    real x = direction.i * 0.01f + origin.x;
    real y = direction.j * 0.01f + origin.y;
    real z = direction.k * 0.01f + origin.z;
    real i = 0.0f - direction.i;
    real j = 0.0f - direction.j;
    real k = 0.0f - direction.k;
    g_4c9798[19] = (k * z + j * y + i * x) * -100.0f;
    g_4c9798[16] = i * 100.0f;
    g_4c9798[17] = j * 100.0f;
    g_4c9798[18] = k * 100.0f;
    real lower = *(real const *)(tag + 0x2c);
    real upper = *(real const *)(tag + 0x30);
    real scale = upper > lower ? 1.0f / (upper - lower) : 1000.0f;
    x = direction.i * lower + origin.x;
    y = direction.j * lower + origin.y;
    z = direction.k * lower + origin.z;
    g_4c9798[23] = 0.0f - (direction.k * z + direction.j * y + direction.i * x) * scale;
    g_4c9798[20] = direction.i * scale;
    g_4c9798[21] = direction.j * scale;
    g_4c9798[22] = direction.k * scale;
    for (long n = 0; n < 8; ++n) g_4c9798[24 + n] = g_485a0c[n];
    D3DDevice_SetVertexShaderConstant(-56, g_4c9798, 9);
}
