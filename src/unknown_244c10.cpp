#include "unknown_11c920.h"
#include "unknown_0259d0.h"

// @flags /O2 /arch:SSE /Gr

struct s_collision_result_1697c0;
bool function_1691a0(long arg_0, dword arg_1, dword arg_2, point3f const *arg_3,
	vector3f const *arg_4, s_collision_result_1697c0 *arg_5);
extern long g_4e7c1c;
extern long g_4e7c20[];

struct s_244c10
{
	long field_00;
	long field_04;
};

class c_244c10
{
public:
	virtual void function_244c10(s_244c10 const *arg_0, void const *arg_1);
	virtual ~c_244c10() {}
	real field_04;
	dword field_08;
	dword field_0c;
	point3f const *field_10;
	vector3f const *field_14;
	bool field_18;
	s_collision_result_1697c0 *field_1c;
};

// @retail 0x244c10
void c_244c10::function_244c10(s_244c10 const *arg_0, void const *arg_1)
{
	long local_0 = arg_0->field_04;
	if (g_4e7c20[(short)local_0] != g_4e7c1c)
	{
		s_collision_result_1697c0 *local_1 = field_1c;
		g_4e7c20[(short)local_0] = g_4e7c1c;
		if (function_1691a0(local_0, field_08, field_0c, field_10, field_14, local_1))
		{
			if (field_08 & 0x10000000)
				field_04 = 0.f;
			else
				field_04 = field_04 > ((real *)field_1c)[1] ? ((real *)field_1c)[1] : field_04;
			field_18 = true;
		}
	}
}
