#include "unknown_11c920.h"
#include "effects.h"

// @flags /O2 /Gr

struct s_particle_impact;
struct s_object_246eb0;
void function_246e60(long tag_index, dword group, s_particle_impact const *impact,
	s_object_246eb0 const *particle, long definition_index);

class c_view_248340
{
public:
	virtual void v0() = 0;
	virtual void v1() = 0;
	virtual dword v2() = 0;
	virtual long v3() = 0;
};

struct s_emitter_248340
{
	short field_00;
	short field_02;
};

// @retail 0x248340
void function_248340(s_particle_system_datum *system, s_object_246eb0 *particle,
	s_emitter_248340 *emitter)
{
	c_view_248340 *definition = (c_view_248340 *)function_137bd0(system->function_1751d0()->tag_index);
	if (definition->v3() != NONE)
	{
		long definition_index = system->function_1751d0()->tag_index;
		long volatile tag_index = definition->v3();
		dword local_0 = definition->v2();
		function_246e60(tag_index, local_0, 0, particle, definition_index);
	}
	*(long *)((byte *)particle + 4) = NONE;
	--emitter->field_02;
}
