// @flags /O1 /Oi /Gr
#include "screen_widgets.h"
#include "loop_allocator.h"
#include <new>

typedef c_profile_mode_screen c_18f4bb;

// @retail 0x18f42d
c_class_1473c9 *__stdcall function_18f42d(s_screen_parameters *arg_0)
{
    c_18f4bb *local_0;
    void *local_1 = function_1a47fd(sizeof(c_18f4bb));
    if (local_1)
        local_0 = ::new (local_1) c_18f4bb(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = false;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}

// @retail 0x18f474
c_class_1473c9 *__stdcall function_18f474(s_screen_parameters *arg_0)
{
    c_18f4bb *local_0;
    void *local_1 = function_1a47fd(sizeof(c_18f4bb));
    if (local_1)
        local_0 = ::new (local_1) c_18f4bb(arg_0->a, arg_0->b, arg_0->user_flags);
    else
        local_0 = 0;
    local_0->m6c = true;
    local_0->mode = true;
    ((c_class_1473c9 *)local_0)->function_147f6d(arg_0);
    return (c_class_1473c9 *)local_0;
}
