// @flags /O2 /arch:SSE /Gr
#include "device_setup_storage.h"
#include "unknown_11c920.h"
#include "unknown_0259d0.h"



real g_485b20;
extern double g_4ba040;
void function_17bc40(real arg_1);
void function_17d690(void);
void __stdcall function_d4db0(long arg_1);
void __stdcall function_18c060(real arg_1);
void function_2118c0(void);
void function_134e00(real arg_1);
void function_14980(void);
bool function_128c0(s_128c0_settings const *arg_1);
bool function_14600(void);
void __stdcall function_39e50(real arg_1);
void function_176f60(real arg_1);

// @retail 0x137ed0
void function_137ed0(real arg_1)
{
	if (arg_1 > 0.0f)
	{
		function_17bc40(arg_1);
		function_17d690();
		function_d4db0(*(long *)&arg_1);
		function_18c060(arg_1);
		function_2118c0();
		function_134e00(arg_1);
		if (g_485ac8 != 0 || g_485aca != 1 || g_485acc != 3 || g_485ad0 != 1)
		{
			function_14980();
			s_128c0_settings local_1;
			*(long *)&local_1 = 0x00010000;
			local_1.quality = 3;
			local_1.extra = 1;
			function_128c0(&local_1);
			function_14600();
		}
		g_485b20 = arg_1;
		function_39e50(arg_1);
	}
	function_176f60(arg_1);
	g_4ba040 += arg_1;
}
