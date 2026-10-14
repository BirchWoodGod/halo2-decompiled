// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0B6320.CPP: filling a buffer with the next bytes of an upload (built
   with SSE, apart from its neighbours in unknown_0b66c0.cpp) */

#include "transaction_storage.h"
#include "unknown_11c920.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>
#include <stdlib.h>
#include "unknown_0b4da0.h"

struct s_upload_file_ab
{
    byte unknown00[0x108];
    HANDLE handle;
    dword position;
};

void function_b65f0(s_upload_ab *upload);

static inline bool upload_read_ab(s_upload_file_ab *file, long position, void *buffer, long count)
{
    if (file->position != position)
    {
        file->position = SetFilePointer(file->handle, position, 0, FILE_BEGIN);
        if (file->position == 0xffffffff)
            return false;
    }
    dword received;
    bool result = false;
    if (ReadFile(file->handle, buffer, count, &received, 0))
    {
        if (received == count)
            result = true;
        else
            SetLastError(0x26);
    }
    file->position += received;
    return result;
}

// @retail 0xb6320
bool function_b6320(s_upload_ab *upload, byte *buffer, long maximum, long *written)
{
    byte *out = buffer;
    long remaining = maximum;
    volatile bool result = false;
    bool valid = upload->kind == 2 || upload->kind == 1;
    unsigned long length = 0;
    char const *path = upload->path;
    for (; length < 0x7f; ++length)
        if (!*path++) break;
    if (length && valid)
    {
        if (!upload->position)
            function_b65f0(upload);
        result = true;
        while (upload->position != upload->ending_length + upload->file_length + upload->part_length + upload->header_length && remaining > 0)
        {
            long position = upload->position;
            long count;
            if (position < upload->header_length)
            {
                count = remaining < upload->header_length - position ? remaining : upload->header_length - position;
                memcpy(out, upload->header + position, count);
            }
            else if ((position -= upload->header_length) < upload->part_length)
            {
                count = remaining < upload->part_length - position ? remaining : upload->part_length - position;
                memcpy(out, upload->part + position, count);
            }
            else if ((position -= upload->part_length) < upload->file_length)
            {
                count = remaining < upload->file_length - position ? remaining : upload->file_length - position;
                if (upload->kind == 1)
                    memcpy(out, upload->memory + position, count);
                else
                {
                    s_upload_file_ab *file = upload->file;
                    bool success = true;
                    if (file->position != position)
                    {
                        file->position = SetFilePointer(file->handle, position, 0, FILE_BEGIN);
                        success = file->position != 0xffffffff;
                    }
                    if (success)
                    {
                        dword received;
                        bool all_received = false;
                        if (ReadFile(file->handle, out, count, &received, 0))
                        {
                            if (received == count) all_received = true;
                            else SetLastError(0x26);
                        }
                        file->position += received;
                        success = all_received;
                    }
                    if (!success)
                    {
                        GetLastError();
                        SetLastError(0);
                        result = false;
                    }
                }
            }
            else if ((position -= upload->file_length) < upload->ending_length)
            {
                count = remaining < upload->ending_length - position ? remaining : upload->ending_length - position;
                memcpy(out, upload->ending + position, count);
            }
            else
            {
                result = false;
                break;
            }
            upload->position += count;
            out += count;
            remaining = maximum - (out - buffer);
        }
        *written = out - buffer;
    }
    return result;
}
