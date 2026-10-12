#ifndef MATERIAL_RECORDS_STORAGE_H
#define MATERIAL_RECORDS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"
#include "index_cache_storage.h"




struct s_material_records_storage
{
    s_44940_entry field_0[850];
};

extern s_material_records_storage g_material_records_storage;
#define g_4ba138 (g_material_records_storage.field_0)
typedef char assert_material_records_offset_0[(offsetof(s_material_records_storage, field_0) == 0x0) ? 1 : -1];

typedef char assert_material_records_size[(sizeof(s_material_records_storage) == 0x6a40) ? 1 : -1];

#endif
