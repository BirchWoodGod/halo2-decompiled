// @flags /O2 /Gr /arch:SSE
// BLOCKED-BY: enabling this body loses matched 0x1cfd10 and 0x277ec0 in the full image.
#if 0
#include "unknown_1cec30.h"

struct s_2792c0_context;
struct s_component_contact_pair;
bool function_2792c0(s_2792c0_context *);
void function_1cfd10(s_component_contact_pair const *, s_havok_component *, real);

// Retail 0x279410, disabled pending the contact-recording callee.
bool __cdecl function_279410(s_component_contact_pair const *pair, hkEntity *moving,
    hkEntity *opposing, real limit, real reduction, real approach)
{
    long slot = havok_entity_property_get(moving, 0x2001);
    long discarded = havok_entity_property_get(moving, 0x2002);
    (void)&discarded;
    long opposite_slot = havok_entity_property_get(opposing, 0x2001);
    s_havok_component *part = havok_component_get(slot);
    byte *entity = (byte *)((s_havok_object_header *)g_4e0300->data)[part->object_index & 0xffff].object;
    byte *second_entity = opposite_slot != NONE && part ?
        (byte *)((s_havok_object_header *)g_4e0300->data)[part->object_index & 0xffff].object : NULL;

    __m128 squared = ((hkRigidBody *)moving)->m_motion->m_linear_velocity.m_quad;
    squared = _mm_mul_ps(squared, squared);
    __m128 accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0x55), squared);
    accumulated = _mm_add_ss(_mm_shuffle_ps(squared, squared, 0xaa), accumulated);
    accumulated = _mm_sqrt_ss(accumulated);
    real absolute_speed;
    _mm_store_ss(&absolute_speed, accumulated);
    real speed = absolute_speed;
    if (((hkRigidBody *)opposing)->m_motion->getType() != 7)
    {
        __m128 difference = _mm_sub_ps(((hkRigidBody *)moving)->m_motion->m_linear_velocity.m_quad,
            ((hkRigidBody *)opposing)->m_motion->m_linear_velocity.m_quad);
        difference = _mm_mul_ps(difference, difference);
        __m128 sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0x55), difference);
        sum = _mm_add_ss(_mm_shuffle_ps(difference, difference, 0xaa), sum);
        sum = _mm_sqrt_ss(sum);
        real relative_speed;
        _mm_store_ss(&relative_speed, sum);
        if (absolute_speed > relative_speed)
            speed = relative_speed;
    }
    if (entity[0xaa] == 0)
    {
        byte *current = (byte *)((s_havok_object_header *)g_4e0300->data)[part->object_index & 0xffff].object;
        if (current[0x3dd] && second_entity && second_entity[0xaa] == 0)
            return false;
        if (!current[0x34b])
        {
            real projected = approach;
            if (projected > speed)
                projected = speed;
            if (projected > limit)
                goto record;
            if (!(speed > limit) || function_2792c0((s_2792c0_context *)pair))
                return false;
            goto record;
        }
    }
    if (!(speed > limit))
        return false;
record:
    function_1cfd10(pair, part, speed - reduction);
    return true;
}

#endif
