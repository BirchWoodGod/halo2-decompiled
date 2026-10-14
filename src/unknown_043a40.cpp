// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "reset_storage.h"
#include "shader_storage.h"
#include <xtl.h>
#include <string.h>
struct s_render_reset_state;
struct s_shader_cache;
extern bool g_4c8780;
extern dword g_4c8784;
extern long g_4c8788, g_4c878c, g_4c8790;
extern byte g_4670bc;
extern long g_4858b8;
extern byte *g_485a80;
extern D3DPIXELSHADERDEF g_484f68;
extern D3DResource *g_509444;
struct s_type_7ba8e9;
void function_16b10(s_render_reset_state *state);
s_type_7ba8e9 *function_137550(long tag, short bitmap);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
void function_14bc0(short target, short element, bool depth);
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
bool function_1ccf0(D3DPIXELSHADERDEF const *program);
dword function_1cc30(long index);
void function_1c620(s_shader_cache *state, long a, long b, long c, byte const *descriptor);
void __stdcall function_1c710(void *state);
byte const g_43f7b9[6] = { 0x2d, 0, 2, 3, 0x10, 0xff };

// @retail 0x43a40
void __stdcall function_43a40(long mode)
{
    (void)&mode;
    g_4c8790 = mode;
    g_4c878c = NONE;
    g_4c8788 = 0;
    g_4c8784 = 0;
    g_4c8780 = true;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_14bc0((short)g_4858b8, 0, true);
    long tag = *(long *)(g_485a80 + 0x64);
    if (tag != NONE)
    {
        s_type_7ba8e9 *bitmap = function_137550(tag, 1);
        if (bitmap) function_14390(0, bitmap, 0.0f);
    }
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    if (!(mode == 6)) {
        function_0222d0(D3DRS_CULLMODE, 0x901);
        function_0222d0(D3DRS_ZENABLE, 2);
        function_0222d0(D3DRS_ZFUNC, 0x203);
        function_0222d0(D3DRS_ZWRITEENABLE, 0);
    } else {
        function_0222d0(D3DRS_CULLMODE, 0);
        function_0222d0(D3DRS_ZENABLE, 0);
        function_0222d0(D3DRS_ZFUNC, 0x207);
        function_0222d0(D3DRS_ZWRITEENABLE, 1);
    }
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    switch (mode)
    {
    case 0:
    case 1:
        g_484f68.PSRGBInputs[0] = 0x8040000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xf150000;
        g_484f68.PSFinalCombinerInputsEFG = 0xc1c1c00;
        break;
    case 2:
    case 3:
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSRGBInputs[1] = ((mode == 2 ? 0xa0 : 0x20) << 8) | 0x81c003c;
        g_484f68.PSRGBOutputs[1] = 0xc00;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        break;
    case 4:
        g_484f68.PSRGBInputs[0] = 0x8040000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xc1c0000;
        break;
    case 5:
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSConstant0[0] = 0xff;
        g_484f68.PSRGBInputs[0] = 0x8040501;
        g_484f68.PSRGBOutputs[0] = 0xcd;
        g_484f68.PSAlphaInputs[0] = 0x38140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSRGBInputs[1] = 0x182b200b;
        g_484f68.PSRGBOutputs[1] = 0xd00;
        g_484f68.PSAlphaInputs[1] = 0x1c0d0000;
        g_484f68.PSAlphaOutputs[1] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xf140000;
        g_484f68.PSFinalCombinerInputsEFG = 0xc0d1c00;
        break;
    default:
        g_484f68.PSTextureModes = 0;
        g_484f68.PSConstant1[3] = 0xff0000;
        g_484f68.PSFinalCombinerInputsABCD = 1;
        break;
    }
    function_1ccf0(&g_484f68);
    function_0222d0(D3DRS_ALPHABLENDENABLE, (dword)mode <= 5);
    if ((dword)mode <= 5)
    {
        function_0222d0(D3DRS_SRCBLEND, mode == 2 || mode == 3 ? 0x306 : 1);
        function_0222d0(D3DRS_DESTBLEND, mode == 2 ? 0x300 : mode == 3 || mode == 4 ? 0 : 0x303);
        function_0222d0(D3DRS_BLENDOP, mode == 4 ? 0x8008 : 0x8006);
    }
    function_0222d0(D3DRS_SOLIDOFFSETENABLE, mode != 6);
    if (mode != 6)
    {
        real offset = -200.0f;
        function_0222d0(D3DRS_POLYGONOFFSETZOFFSET, *(dword *)&offset);
        function_0222d0(D3DRS_POLYGONOFFSETZSLOPESCALE, 0);
    }
    function_1cc30(3);
    function_1c620((s_shader_cache *)g_51f0f0, (long)g_509444, 0, 16, g_43f7b9);
    function_1c710(g_51f0f0);
}
