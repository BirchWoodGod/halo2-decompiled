// @flags /O2 /arch:SSE /Gr
// relevance of one object to the observers, and a value scaled from it
#include "unknown_11c920.h"
#include "unknown_08b110.h"
#include "unknown_0259d0.h"
#include "unknown_19ec40.h"

struct s_relevance_observers;
real function_aa7c0(s_relevance_observers const *observers, point3f const *position, real maximum_distance);

struct s_relevance_scale
{
	real maximum_distance;
	real upper_relevance;
	real lower_relevance;
	long upper_value;
	long lower_value;
};

s_relevance_scale g_4cecf8;

__declspec(noinline) void __stdcall function_82a40(long object_index, s_relevance_observers const *observers, real *relevance, long *value);

// @retail 0x82a40
void __stdcall function_82a40(long object_index, s_relevance_observers const *observers, real *relevance, long *value)
{
	point3f position;
	function_b9dd0(object_index, &position);
	real result = function_aa7c0(observers, &position, g_4cecf8.maximum_distance);
	*relevance = result;
	real upper = g_4cecf8.upper_relevance;
	long near_value = g_4cecf8.lower_value;
	long far_value = g_4cecf8.upper_value;
	real lower = g_4cecf8.lower_relevance;
	if (result >= upper)
	{
		*value = far_value;
		return;
	}
	long scaled;
	if (result <= lower)
		scaled = near_value;
	else
		scaled = (long)((real)near_value - (result - lower) / (upper - lower) * (real)(near_value - far_value));
	*value = scaled;
}
