#ifndef CLUSTER_MARKS_STORAGE_H
#define CLUSTER_MARKS_STORAGE_H

#include <stddef.h>
#include "unknown_11c920.h"

/* the cluster mark stamp at 0x4e7414, the per-cluster marks at 0x4e7418 and
   the busy flag at 0x4e7411 set around cluster searches, as one structure:
   the searches read the stamp and the marks, so the flag stores around them
   stay */
struct s_cluster_marks_storage
{
    byte unknown_0;
    bool field_1;
    byte unknown_2[0x2];
    long field_4;
    long field_8[0x200];
};

extern s_cluster_marks_storage g_cluster_marks_storage;
#define g_4e7411 (g_cluster_marks_storage.field_1)
#define g_4e7414 (g_cluster_marks_storage.field_4)
#define g_4e7418 (g_cluster_marks_storage.field_8)
typedef char assert_cluster_marks_offset_1[(offsetof(s_cluster_marks_storage, field_1) == 0x1) ? 1 : -1];
typedef char assert_cluster_marks_offset_4[(offsetof(s_cluster_marks_storage, field_4) == 0x4) ? 1 : -1];
typedef char assert_cluster_marks_offset_8[(offsetof(s_cluster_marks_storage, field_8) == 0x8) ? 1 : -1];

typedef char assert_cluster_marks_size[(sizeof(s_cluster_marks_storage) == 0x808) ? 1 : -1];

#endif
