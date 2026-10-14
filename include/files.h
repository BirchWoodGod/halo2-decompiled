/* FILES.H: file references and file operations
   (unknown_136710.cpp, unknown_1367d0.cpp, unknown_136d50.cpp) */

#ifndef FILES_H
#define FILES_H

#include "unknown_11c920.h"
#include <xtl.h>

#define FILE_REFERENCE_SIGNATURE 0x66696c6f

struct s_type_acf665
{
	dword signature;
	word flags;
	short location;
	char path[256];
	HANDLE handle;
	dword position;
};

void function_137320(char *path, const char *name);
void function_1373c0(char *path);
bool function_1367d0(s_type_acf665 *file);
bool function_136860(s_type_acf665 *file);
bool function_136970(s_type_acf665 *file, dword flags, dword *error);
bool function_136bb0(s_type_acf665 *file);
bool function_136bf0(s_type_acf665 *file, dword position, bool silent);
bool function_136ca0(s_type_acf665 *file, dword size, void *buffer, bool silent);
bool function_136d00(s_type_acf665 *file, const void *buffer, dword size);
bool function_136d50(s_type_acf665 *file, dword position, dword size, bool silent, void *buffer);
bool function_136d90(s_type_acf665 *file, dword position, dword size, const void *buffer);

#endif
