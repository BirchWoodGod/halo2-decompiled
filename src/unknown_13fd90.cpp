// @flags /O2 /Gr
/* UNKNOWN_13FD90.CPP: unicode character classification and UTF-8 conversion */

#include "unknown_11c920.h"
#include "unknown_13fd90.h"

// @retail 0x13fd90
bool function_13fd90(utf32 character)
{
	long c = character.value;

	if ((c >= 0x1100 && c <= 0x11ff) ||
		(c >= 0x3000 && c <= 0xd7af) ||
		(c >= 0xf900 && c <= 0xfaff) ||
		(c >= 0xff00 && c <= 0xffdc))
	{
		return true;
	}

	return false;
}

// @retail 0x13fde0
bool function_13fde0(utf32 character)
{
	bool result = false;

	switch ((dword)character.value)
	{
		case 0x21: case 0x22: case 0x25: case 0x29: case 0x2c: case 0x2e: case 0x3a: case 0x3b:
		case 0x3f: case 0x5d: case 0x7d: case 0xb0: case 0xb7: case 0x2013: case 0x2014: case 0x2019:
		case 0x201d: case 0x2022: case 0x2026: case 0x2027: case 0x2032: case 0x2033: case 0x2103: case 0x3001:
		case 0x3002: case 0x3009: case 0x300b: case 0x300d: case 0x300f: case 0x3011: case 0x3015: case 0x301e:
		case 0x3041: case 0x3043: case 0x3045: case 0x3047: case 0x3049: case 0x3063: case 0x3083: case 0x3085:
		case 0x3087: case 0x308e: case 0x30a1: case 0x30a3: case 0x30a5: case 0x30a7: case 0x30a9: case 0x30c3:
		case 0x30e3: case 0x30e5: case 0x30e7: case 0x30ee: case 0x30fc: case 0xfe30: case 0xfe50: case 0xfe51:
		case 0xfe52: case 0xfe54: case 0xfe55: case 0xfe56: case 0xfe57: case 0xfe5a: case 0xfe5c: case 0xfe5e:
		case 0xff01: case 0xff05: case 0xff09: case 0xff0c: case 0xff0e: case 0xff1a: case 0xff1b: case 0xff1f:
		case 0xff3d: case 0xff5d: case 0xff61: case 0xff64: case 0xff70: case 0xff9e: case 0xff9f: case 0xffe0:
		result = true;
		break;
	}

	return result;
}

// @retail 0x140260
bool function_140260(utf32 character)
{
	bool result = false;

	switch ((dword)character.value)
	{
		case 0x22: case 0x24: case 0x28: case 0x5b: case 0x5c: case 0x7b: case 0x2018: case 0x201c:
		case 0x2035: case 0x3008: case 0x300a: case 0x300c: case 0x300e: case 0x3010: case 0x3014: case 0x301d:
		case 0xfe59: case 0xfe5b: case 0xfe5d: case 0xff04: case 0xff08: case 0xff3b: case 0xff5b: case 0xffe1:
		case 0xffe5: case 0xffe6:
		result = true;
		break;
	}

	return result;
}

// @retail 0x140420
bool function_140420(word c)
{
	bool result = false;

	if (c >= 0xe000 && c <= 0xf8ff)
	{
		result = true;
	}

	return result;
}

// @retail 0x140440
void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count)
{
	const byte *bytes = (const byte *)source;
	long source_index = 0;
	long destination_index = 0;

	while (source[source_index])
	{
		dword value;
		long length;
		bool valid = true;
		dword lead = bytes[source_index];

		if (!(lead & 0x80))
		{
			value = lead;
			length = 1;
		}
		else if ((lead & 0xe0) == 0xc0)
		{
			value = lead & ~0xe0u;
			length = 2;
		}
		else if ((lead & 0xf0) == 0xe0)
		{
			value = lead & ~0xf0u;
			length = 3;
		}
		else if ((lead & 0xf8) == 0xf0)
		{
			value = lead & ~0xf8u;
			length = 4;
		}
		else
		{
			valid = false;
		}

		if (valid)
		{
			for (long i = 1; valid && i < length; i++)
			{
				dword continuation = bytes[source_index + i];

				value = (value << 6) | (continuation & ~0xc0u);
				if ((continuation & 0xc0) != 0x80)
				{
					valid = false;
				}
			}
		}

		if (valid)
		{
			source_index += length;
		}
		else
		{
			source_index++;
		}

		if (valid && destination_index < destination_count)
		{
			if (value <= (dword)NONE)
			{
				destination[destination_index] = (word)value;
			}
			else
			{
				destination[destination_index] = '?';
			}
			destination_index++;
		}
	}

	if (destination_index < destination_count)
	{
		destination[destination_index] = 0;
	}
	else if (destination_index > 0)
	{
		destination[destination_index - 1] = 0;
	}
}

// @retail 0x1405a0
long utf8_encode_character(dword value, byte *buffer, long buffer_size)
{
	long length;
	dword first_byte;

	if (value <= 0x7f)
	{
		first_byte = value;
		length = 1;
	}
	else if (value <= 0x7ff)
	{
		first_byte = (value | 0x3000) >> 6;
		length = 2;
	}
	else if (value <= 0xffff)
	{
		first_byte = (value | 0xe0000) >> 12;
		length = 3;
	}
	else if (value <= 0x1fffff)
	{
		first_byte = (value | 0x3c00000) >> 18;
		length = 4;
	}
	else
	{
		length = 0;
	}

	if (length > 0 && buffer_size > 0)
	{
		buffer[0] = (byte)first_byte;
	}

	for (long i = 1; i < buffer_size && i < length; i++)
	{
		buffer[i] = (byte)(((value >> (6 * (length - i) - 6)) & 0x3f) | 0x80);
	}

	return length;
}

void utf16_string_to_utf8_string(const word *source, char *destination, long destination_size);

static __forceinline void utf16_string_write_utf8(const word *source, char *destination, long destination_size)
{
	long destination_index = 0;

	for (; *source; source++)
	{
		long remaining = destination_size - destination_index;
		long length = utf8_encode_character(*source, (byte *)destination + destination_index, remaining);

		destination_index += length <= remaining ? length : remaining;
	}

	if (destination_index < destination_size)
	{
		destination[destination_index] = 0;
	}
	else if (destination_index > 0)
	{
		destination[destination_index - 1] = 0;
	}
}

// @retail 0x140650
void utf16_string_to_utf8_string(const word *source, char *destination, long destination_size)
{
	utf16_string_write_utf8(source, destination, destination_size);
}
