// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1D6560.CPP: per-object physics setup: which entries of an object's
   physics model hold (judged by its damage sections), and the layer each of
   its rigid bodies collides in */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"

/* a damage section an entry depends on (0xc bytes) */
struct s_physics_entry_section
{
	short key_a;
	short key_b;
	byte flags;
	byte unknown05[0xc - 5];
};

/* an entry of the physics model (0x18 bytes) */
struct s_physics_entry
{
	byte unknown00[8];
	long section_count;
	s_physics_entry_section *sections;
	byte unknown10[0x18 - 0x10];
};

/* a rigid body of the physics model (0x90 bytes) */
struct s_physics_rigid_body
{
	byte unknown00[0x18];
	byte flags;
	byte unknown19[0x90 - 0x19];
};

struct s_physics_model
{
	byte unknown00[0x30];
	long entry_count;
	s_physics_entry *entries;
	byte unknown38[4];
	s_physics_rigid_body *rigid_bodies;
};

struct s_physics_model_owner
{
	byte unknown00[0x48];
	s_physics_model *model;
};

struct s_physics_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

bool function_db540(long object_index, long key_a, long key_b);

/* sets the bit of each entry that has no sections, or one of whose flagged
   sections is gone; clears the others */
PRIVATE __forceinline bool function_1d6561(s_havok_component const *arg_0, s_physics_entry_section const *arg_1)
{
    long local_0 = *(volatile long const *)&arg_0->object_index;
    return function_db540(local_0, arg_1->key_a, arg_1->key_b);
}

// @retail 0x1d6560
void __stdcall function_1d6560(s_havok_component *component, s_physics_model_owner *owner, dword *mask)
{
	long i;

	for (i = 0; i < owner->model->entry_count; i++)
	{
		s_physics_entry *entry = &owner->model->entries[i];

		mask[i >> 5] &= ~(1 << (i & 31));
		if (entry->section_count == 0)
		{
			mask[i >> 5] |= 1 << (i & 31);
		}
		else
		{
			long j;

			for (j = 0; j < entry->section_count; j++)
			{
				s_physics_entry_section *section = &entry->sections[j];

				if ((section->flags & 1) && !function_1d6561(component, section))
				{
					mask[i >> 5] |= 1 << (i & 31);
					break;
				}
			}
		}
	}
}

/* the collision layer of one of the object's rigid bodies, by object type */
// @retail 0x1d69d0
long function_1d69d0(s_havok_component *component, long kind, s_physics_model_owner *owner, long level, long rigid_body_index)
{
	s_physics_object_header *header = &((s_physics_object_header *)g_4e0300->data)[component->object_index & 0xffff];
	long type = header->type;
	s_physics_rigid_body *rigid_body = &owner->model->rigid_bodies[rigid_body_index];
	long result;

	if (kind == 7)
	{
		switch (type)
		{
		case 1:
			result = 0xc;
			break;
		default:
			result = 1;
			break;
		}
		return result;
	}
	switch (type)
	{
	case 11:
		if (level <= 1)
		{
			result = 4;
		}
		else
		{
			result = level >= 5 ? 5 : 3;
		}
		break;
	case 1:
		result = (rigid_body->flags & 8) ? 14 : 12;
		break;
	case 0:
		result = (bool)(((dword)header->object[0x10a] >> 2) & 1) ? 11 : 9;
		break;
	case 7:
		if (kind == 6)
		{
			result = 8;
		}
		else
		{
			result = (rigid_body->flags & 8) ? 7 : 6;
		}
		break;
	default:
		__assume(0);
	}
	return result;
}

struct s_physics_graph_body
{
    byte unknown00[0x18];
    byte flags;
    byte unknown19[3];
    short replacement;
    byte unknown1e[0x90 - 0x1e];
};

struct s_physics_graph_entry
{
    byte unknown00[4];
    short groups[2];
    long section_count;
    s_physics_entry_section *sections;
    byte unknown10[8];
};

struct s_physics_graph_model
{
    byte unknown00[0x30];
    long entry_count;
    s_physics_graph_entry *entries;
    long body_count;
    s_physics_graph_body *bodies;
};

struct s_physics_body_group
{
    char bodies[64];
    long count;
};

struct s_physics_body_groups
{
    long unknown00;
    s_physics_body_group groups[257];
};

bool function_db4c0(long object_index, long key_a, long key_b);

// @retail 0x1d6630
void function_1d6630(s_havok_component *component, s_physics_model_owner *owner, char const *kinds,
    dword const *entry_mask, s_physics_body_groups *groups, void *context, long parent, long current,
    char *output, long *output_count, void *other_context, dword *visited, bool *active)
{
    (void)&owner;
    (void)&kinds;
    (void)&entry_mask;
    (void)&groups;
    (void)&context;
    (void)&parent;
    (void)&current;
    (void)&output;
    (void)&output_count;
    (void)&other_context;
    (void)&visited;
    (void)&active;
    bool replace = !(bool)((component->unknown04 >> 13) & 1);
    if (groups->groups[current == NONE ? 256 : current].count > 0)
    {
        for (long i = 0; i < groups->groups[current == NONE ? 256 : current].count; ++i)
        {
            long body_index = groups->groups[current == NONE ? 256 : current].bodies[i];
            s_physics_graph_model *model = (s_physics_graph_model *)owner->model;
            s_physics_graph_body *body = &model->bodies[body_index];
            bool replaced = false;
            bool available = false;
            if (body->flags & 0x20)
            {
                long replacement = body->replacement;
                long bounded = replacement < 0 ? 0 : replacement > model->body_count - 1 ? model->body_count - 1 : replacement;
                if (bounded == replacement)
                {
                    available = true;
                    if (replace)
                    {
                        replaced = true;
                        body_index = replacement;
                    }
                }
            }
            output[(*output_count)++] = (char)body_index;
            if (replaced)
                component->unknown04 |= 0x100000;
            if (available)
                component->unknown04 |= 0x80000;
        }
        if (current != NONE)
            visited[current >> 5] |= 1 << (current & 31);
        for (long entry_index = 0; entry_index < owner->model->entry_count; ++entry_index)
        {
            s_physics_graph_entry *entry = &((s_physics_graph_model *)owner->model)->entries[entry_index];
            if ((entry_mask[entry_index >> 5] & (1 << (entry_index & 31))) &&
                (entry->groups[0] == current || entry->groups[1] == current))
            {
                long next = entry->groups[0] == current ? entry->groups[1] : entry->groups[0];
                long first_body = NONE;
                if (next != NONE && groups->groups[next].count > 0)
                    first_body = groups->groups[next].bodies[0];
                if (first_body != NONE && (kinds[first_body] == 6 || (visited[next >> 5] & (1 << (next & 31)))))
                    continue;
                if (next == NONE)
                {
                    for (long j = 0; j < entry->section_count; ++j)
                    {
                        s_physics_entry_section *section = &entry->sections[j];
                        if (!function_db540(component->object_index, section->key_a, section->key_b) &&
                            !function_db4c0(component->object_index, section->key_a, section->key_b))
                        {
                            *active = true;
                            break;
                        }
                    }
                }
                else
                {
                    if (groups->groups[next].count > 0)
                    {
                        if (entry->section_count == 0)
                        {
                            long this_body = groups->groups[current == NONE ? 256 : current].bodies[0];
                            long next_body = groups->groups[next].bodies[0];
                            if (kinds[this_body] == 7 || kinds[next_body] == 7)
                                continue;
                        }
                        else
                        {
                            long j;
                            for (j = 0; j < entry->section_count; ++j)
                            {
                                s_physics_entry_section *section = &entry->sections[j];
                                if (!function_db540(component->object_index, section->key_a, section->key_b) &&
                                    !function_db4c0(component->object_index, section->key_a, section->key_b))
                                    break;
                            }
                            if (j >= entry->section_count)
                                continue;
                        }
                    }
                    if (first_body != NONE)
                        function_1d6630(component, owner, kinds, entry_mask, groups, context, current, next,
                            output, output_count, other_context, visited, active);
                }
            }
        }
    }
}


#include "havok_reference.h"
#include <new>

struct s_physics_mass
{
    real volume;
    real mass;
    byte unknown08[8];
    hkVector4 center;
    hkRotation inertia;
};

struct s_physics_mass_entry
{
    s_physics_mass mass;
    hkTransform transform;
};

struct s_physics_mass_array
{
    s_physics_mass_entry *data;
    long size;
    long capacity;
};

class c_physics_allocation_view
{
public:
    virtual void slot0() {}
    virtual void slot1() {}
    virtual void slot2() {}
    virtual void slot3() {}
    virtual void *allocate(long bytes, long category) { return NULL; }
    virtual void release(void *data, long bytes, long category) {}
};

class c_physics_shape_view : public c_havok_reference_counted
{
public:
    virtual void slot1() {}
    virtual void slot2() {}
    virtual void slot3() {}
    virtual void slot4() {}
    virtual long type() { return 0; }
};

struct c_child_transform : c_havok_reference_counted
{
    dword user;
    long field_c;
    hkTransform transform;
    c_child_transform(c_havok_reference_counted *child);
};

class c_physics_shape_list
{
public:
    c_physics_shape_list(c_havok_reference_counted **shapes, long count);
};

void __cdecl function_2dba80(s_physics_mass_array const *array, s_physics_mass *result);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *out);
extern transform4x3f *g_4687d0;

PRIVATE inline void physics_mass_frame(transform4x3f const *matrix, hkTransform *out)
{
    real *columns = (real *)out;
    columns[0] = matrix->forward.i; columns[1] = matrix->forward.j; columns[2] = matrix->forward.k; columns[3] = 0;
    columns[4] = matrix->left.i; columns[5] = matrix->left.j; columns[6] = matrix->left.k; columns[7] = 0;
    columns[8] = matrix->up.i; columns[9] = matrix->up.j; columns[10] = matrix->up.k; columns[11] = 0;
    columns[12] = matrix->position.x; columns[13] = matrix->position.y; columns[14] = matrix->position.z; columns[15] = 0;
}

PRIVATE inline void physics_mass_identity(hkTransform *out)
{
    __m128 zero = _mm_setzero_ps();
    out->m_rotation.m_col0.m_quad = zero;
    out->m_rotation.m_col1.m_quad = zero;
    out->m_rotation.m_col2.m_quad = zero;
    ((real *)&out->m_rotation.m_col0)[0] = 1.0f;
    ((real *)&out->m_rotation.m_col1)[1] = 1.0f;
    ((real *)&out->m_rotation.m_col2)[2] = 1.0f;
    out->m_translation.m_quad = zero;
}

PRIVATE inline void physics_mass_copy(byte const *body, s_physics_mass *mass)
{
    mass->volume = 0;
    mass->mass = *(real const *)(body + 0x3c);
    mass->center = *(hkVector4 const *)(body + 0x40);
    mass->inertia = *(hkRotation const *)(body + 0x50);
}

// @retail 0x1d5ec0
c_havok_reference_counted *__stdcall function_1d5ec0(s_havok_component *component,
    s_physics_model_owner *owner, char const *kinds, char *output, long *output_count,
    s_physics_body_groups *groups, long current, dword *visited, real *mass,
    hkVector4 *center, hkRotation *inertia, bool *active, long *maximum,
    bool *mixed, bool *special)
{
    dword entry_mask[128];
    *output_count = 0;
    *active = false;
    *mixed = false;
    function_1d6560(component, owner, entry_mask);
    function_1d6630(component, owner, kinds, entry_mask, groups, (void *)current, NONE,
        current, output, output_count, (void *)64, visited, active);
    if (*output_count <= 0)
        return NULL;
    long first_index = output[0];
    bool preserve_wrapper = kinds[first_index] == 7 || kinds[first_index] == 6;
    byte *bodies = (byte *)owner->model->rigid_bodies;
    byte *first = bodies + first_index * 0x90;
    c_havok_reference_counted *result;
    long flagged = ((dword)first[0x18] >> 2) & 1;
    *maximum = *(short *)(first + 0x1e);
    if (*output_count == 1)
    {
        *mass = *(real *)(first + 0x3c);
        *center = *(hkVector4 *)(first + 0x40);
        *inertia = *(hkRotation *)(first + 0x50);
        result = *(c_havok_reference_counted **)(first + 0x38);
        result->reference_count++;
        if (first[0x8e] & 1)
            *special = true;
    }
    else
    {
        byte *model = *(byte **)((byte *)owner + 0x44);
        byte *render = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
        byte *nodes = *(byte **)(render + 0x4c);
        transform4x3f const *root = *(short *)first == NONE ? g_4687d0 :
            (transform4x3f *)(nodes + *(short *)first * 0x60 + 0x28);
        c_havok_reference_counted *shapes[64];
        c_physics_shape_view *shape = *(c_physics_shape_view **)(first + 0x38);
        if (!preserve_wrapper && shape->type() == 0x13)
            shape = *(c_physics_shape_view **)((byte *)shape + 0xc);
        shapes[0] = shape;
        shape->reference_count++;
        if (first[0x8e] & 1)
            *special = true;
        c_physics_allocation_view *allocator = (c_physics_allocation_view *)g_480118;
        s_physics_mass_array array;
        array.data = (s_physics_mass_entry *)allocator->allocate(*output_count * 0x90, 0x12);
        array.size = array.capacity = *output_count;
        s_physics_mass aggregate;
        aggregate.volume = aggregate.mass = 0;
        aggregate.center.m_quad = _mm_setzero_ps();
        aggregate.inertia.m_col0.m_quad = _mm_setzero_ps();
        aggregate.inertia.m_col1.m_quad = _mm_setzero_ps();
        aggregate.inertia.m_col2.m_quad = _mm_setzero_ps();
        physics_mass_identity(&array.data[0].transform);
        physics_mass_copy(first, &array.data[0].mass);
        for (long i = 1; i < *output_count; i++)
        {
            byte *body = (byte *)owner->model->rigid_bodies + output[i] * 0x90;
            c_physics_shape_view *child = *(c_physics_shape_view **)(body + 0x38);
            bool transformed = *(short *)first != *(short *)body;
            hkTransform transform;
            if (transformed)
            {
                transform4x3f inverse, relative;
                function_141590((transform4x3f *)(nodes + *(short *)body * 0x60 + 0x28), &inverse);
                function_142a60(root, &inverse, &relative);
                physics_mass_frame(&relative, &transform);
                if (child->type() == 0x15)
                {
                    transform.setMulEq(*(hkTransform *)((byte *)child + 0x10));
                    child = *(c_physics_shape_view **)((byte *)child + 0xc);
                }
            }
            else
                physics_mass_identity(&transform);
            if (!preserve_wrapper && child->type() == 0x13)
                child = *(c_physics_shape_view **)((byte *)child + 0xc);
            if (transformed)
            {
                void *storage = ((c_physics_allocation_view *)g_480118)->allocate(0x50, 0x22);
                c_child_transform *wrapper = storage ? new (storage) c_child_transform(child) : NULL;
                wrapper->transform = transform;
                child = (c_physics_shape_view *)wrapper;
            }
            else
                child->reference_count++;
            if (body[0x8e] & 1)
                *special = true;
            shapes[i] = child;
            array.data[i].transform = transform;
            physics_mass_copy(body, &array.data[i].mass);
            long value = *(short *)(body + 0x1e);
            *maximum = *maximum > value ? *maximum : value;
            flagged += ((dword)body[0x18] >> 2) & 1;
        }
        function_2dba80(&array, &aggregate);
        *mass = aggregate.mass;
        *center = aggregate.center;
        *inertia = aggregate.inertia;
        void *storage = ((c_physics_allocation_view *)g_480118)->allocate(0x38, 0x22);
        *(word *)((byte *)storage + 4) = 0x38;
        result = (c_havok_reference_counted *)new (storage) c_physics_shape_list(shapes, *output_count);
        for (long i = 0; i < *output_count; i++)
            havok_reference_remove(shapes[i]);
        if (!(array.capacity & 0x80000000))
            ((c_physics_allocation_view *)g_480118)->release(array.data, (array.capacity & 0x7fffffff) * 0x90, 0x12);
    }
    *mixed = flagged != *output_count;
    return result;
}
