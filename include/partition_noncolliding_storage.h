#ifndef PARTITION_NONCOLLIDING_STORAGE_H
#define PARTITION_NONCOLLIDING_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"




struct s_partition_noncolliding_storage
{
    void * field_0;
    void * field_4;
    void * field_8;
};

extern s_partition_noncolliding_storage g_partition_noncolliding_storage;
#define g_4de2d4 (g_partition_noncolliding_storage.field_0)
#define g_4de2d8 (g_partition_noncolliding_storage.field_4)
#define g_4de2dc (g_partition_noncolliding_storage.field_8)
typedef char assert_partition_noncolliding_offset_0[(offsetof(s_partition_noncolliding_storage, field_0) == 0x0) ? 1 : -1];
typedef char assert_partition_noncolliding_offset_4[(offsetof(s_partition_noncolliding_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_partition_noncolliding_offset_8[(offsetof(s_partition_noncolliding_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_partition_noncolliding_size[(sizeof(s_partition_noncolliding_storage) == 0xc) ? 1 : -1];

#endif
