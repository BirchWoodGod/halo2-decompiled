// stubs for the game functions that lane W's code calls and that are not
// decompiled yet

#include "unknown_11c920.h"
#include "saved_frames_storage.h"
#include "visibility_storage.h"

#include "unknown_0259d0.h"

struct s_frame_view_2c560;








struct s_2f970_view;
struct s_speed_result;


struct s_bit_vector_pool;



struct s_visibility_plane_query;
struct s_visibility_query_list;
// @stub 0x2dd930
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first)
{
}


struct s_visibility_sphere_query;
// @stub 0x2ddbd0
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first)
{
}


struct transform4x3f;


class __declspec(align(16)) c_z_capsule_temp
{
public:
    c_z_capsule_temp(void const *first, void const *second, real radius);
    ~c_z_capsule_temp();
    byte unknown00[0x30];
};

// @stub 0xdc370
c_z_capsule_temp::~c_z_capsule_temp() {}


// @stub 0x14b60
void __stdcall function_14b60(dword, dword, real, byte) {}


struct s_view_camera;
struct s_view_source;


// @stub 0x132d0
void __stdcall function_132d0(s_frame_view_2c560 const *) {}

// @stub 0x3bd00
bool __stdcall function_3bd00(real, dword) { return true; }

struct s_depth_parameters_13450;
// @stub 0x13450
bool function_13450(s_depth_parameters_13450 const *parameters) { return false; }

// @stub 0x289e0
bool function_289e0() { return false; }
