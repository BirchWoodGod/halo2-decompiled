// stubs for game functions not decompiled yet, called by bipeds.cpp
#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_animation_state;
struct s_havok_component;
struct s_havok_component_element0c;
struct s_biped_physics_input;
struct s_biped_physics_output;
struct s_biped_physics_result;
struct s_biped_physics_move;
struct s_biped_ground_collision;







// @stub 0xe5d50
void __stdcall function_e5d50(long arg_159e6d, long target_index, vector3f *offset, point3f *point) { }

// @stub 0xe57e0
void __stdcall function_e57e0(long arg_159e6d) { }

// @stub 0xe63b0
bool function_e63b0(vector3f const *aim, bool flag) { return 0; }




struct s_state_c570;
// @stub 0x1ec690
void function_1ec690(byte const *input, s_state_c570 *state, byte *output) {}

class __declspec(align(16)) c_z_capsule_temp
{
public:
 c_z_capsule_temp(void const *first, void const *second, real radius);
 ~c_z_capsule_temp();
 byte unknown00[0x30];
};
// @stub 0x2de0a0
c_z_capsule_temp::c_z_capsule_temp(void const *first, void const *second, real radius) {}

