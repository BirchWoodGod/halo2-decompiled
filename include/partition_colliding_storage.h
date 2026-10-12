#ifndef PARTITION_COLLIDING_STORAGE_H
#define PARTITION_COLLIDING_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_partition_colliding_storage
{
    void * field_0;
    void * field_4;
    void * field_8;
};

extern s_partition_colliding_storage g_partition_colliding_storage;
#define g_4de2e0 (g_partition_colliding_storage.field_0)
#define g_4de2e4 (g_partition_colliding_storage.field_4)
#define g_4de2e8 (g_partition_colliding_storage.field_8)
typedef char assert_partition_colliding_offset_0[(offsetof(s_partition_colliding_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_partition_colliding_offset_4[(offsetof(s_partition_colliding_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_partition_colliding_offset_8[(offsetof(s_partition_colliding_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_partition_colliding_size[(sizeof(s_partition_colliding_storage) == 0xc) ? 1 : -1];

#endif
