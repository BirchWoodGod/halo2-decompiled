// @flags /O2 /Gr
/* UNKNOWN_136D50.CPP: file reads and writes at a position (the same object as
   unknown_1367d0.cpp in retail) */

#include "unknown_11c920.h"
#include <xtl.h>
#include "files.h"

#pragma inline_depth(0)
// @retail 0x136d50
bool function_136d50(s_type_acf665 *file, dword position, dword size, bool silent, void *buffer)
{
	return function_136bf0(file, position, silent) && function_136ca0(file, size, buffer, silent);
}
#pragma inline_depth(255)

// @retail 0x136d90
bool function_136d90(s_type_acf665 *file, dword position, dword size, const void *buffer)
{
	bool success = true;
	if (file->position != position)
	{
		file->position = SetFilePointer(file->handle, position, NULL, FILE_BEGIN);
		success = file->position != INVALID_SET_FILE_POINTER;
		if (!success)
		{
			GetLastError();
			SetLastError(0);
		}
	}
	return success && function_136d00(file, buffer, size);
}
