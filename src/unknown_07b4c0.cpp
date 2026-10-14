// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "network_qos.h"
#include "bitstream.h"
#include "unknown_07f720.h"
#include <xtl.h>
#include <string.h>

/* QoS (quality of service) probe handles: a data array of 8-byte elements
   {salt, state, XNQOS *} kept in g_4cf8d8 */
struct s_qos_handle
{
	short salt;
	short state;
	XNQOS *qos;
};

struct s_qos_target
{
	XNKID kid;
	XNKEY key;
	XNADDR xna;
};

struct s_session_description
{
	byte unknown0[2];
	short field2;
	long field4;
	long field8;
	long fieldc;
	short field10;
	short field12;
	short field14;
	byte unknown16[0x7e];
	short field94;
	short field96;
	short field98;
	short field9a;
	short field9c;
	short field9e;
	byte unknowna0[8];
	long fielda8;
	byte unknownac[8];
	long fieldb4;
	long fieldb8;
	short fieldbc;
	byte unknownbe[0x622];
	dword field6e0;
};


static const long g_440188[2] = { 8, 0 };
static const long g_440190[2] = { 16384, 4096 };

/* stand-in: retail reloads g_4cf8d8 after record_pool_allocate because some function not yet decompiled
   (probably the QoS initializer) takes its address; remove this once that function exists */
PRIVATE s_record_pool **qos_handle_array(void)
{
	return &g_4cf8d8;
}

/* retail inlines this into all four callers; with __inline alone, LTCG stopped
   inlining it once lane A round 3's unknown_272b70 code joined the program */
static __forceinline s_qos_handle *qos_handle_get(long handle)
{
	s_record_pool *data = g_4cf8d8;
	long index = handle & 0xffff;

	if (index < data->high_water_index)
	{
		s_qos_handle *element = (s_qos_handle *)(data->data + data->size * index);

		if (element->salt && element->salt == (handle >> 16))
		{
			return element;
		}
	}

	return 0;
}

// @retail 0x7b4c0
long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets)
{
	long handle = NONE;

	if (g_4cf8d4)
	{
		long probes = g_440188[kind];
		long bandwidth = bits_per_second;
		XNQOS *qos;
		const XNKEY *keys[64];
		const XNKID *kids[64];
		const XNADDR *addresses[64];

		if (bits_per_second == NONE)
		{
			bandwidth = g_440190[kind];
		}

		if (count > 64)
		{
			count = 64;
		}

		for (long i = 0; i < count; i++)
		{
			kids[i] = &targets[i].kid;
			keys[i] = &targets[i].key;
			addresses[i] = &targets[i].xna;
		}

		if (XNetQosLookup(count, addresses, kids, keys, 0, 0, 0, probes, bandwidth, 0, 0, &qos) == 0)
		{
			handle = record_pool_allocate(g_4cf8d8);
			if (handle != NONE)
			{
				s_qos_handle *element = &((s_qos_handle *)g_4cf8d8->data)[handle & 0xffff];

				element->state = 1;
				element->qos = qos;
			}
			else
			{
				XNetQosRelease(qos);
			}
		}
	}

	return handle;
}

// @retail 0x7b5e0
long qos_service_lookup(void)
{
	long handle = NONE;

	if (g_4cf8d4)
	{
		XNQOS *qos;

		if (XNetQosServiceLookup(0, 0, &qos) == 0)
		{
			handle = record_pool_allocate(g_4cf8d8);
			if (handle != NONE)
			{
				s_qos_handle *element = &((s_qos_handle *)g_4cf8d8->data)[handle & 0xffff];

				element->state = 1;
				element->qos = qos;
			}
			else
			{
				XNetQosRelease(qos);
			}
		}
	}

	return handle;
}

// @retail 0x7b650
void qos_release(long handle)
{
	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			if (XNetQosRelease(element->qos) == 0)
			{
				element->qos = 0;
			}

			record_pool_release(g_4cf8d8, handle);
		}
	}
}

// @retail 0x7b6c0
bool qos_is_complete(long handle)
{
	bool result = true;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			result = element->qos->cxnqosPending == 0;
		}
	}

	return result;
}

// @retail 0x7b720
long qos_target_status(long handle, long index)
{
	long result = 3;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			XNQOSINFO *info = &element->qos->axnqosinfo[index];

			if (info->bFlags & XNET_XNQOSINFO_COMPLETE)
			{
				if (info->bFlags & XNET_XNQOSINFO_TARGET_DISABLED)
				{
					result = 4;
				}
				else
				{
					result = (info->bFlags & XNET_XNQOSINFO_TARGET_CONTACTED) ? 5 : 3;
				}
			}
			else if (info->bFlags & XNET_XNQOSINFO_PARTIAL_COMPLETE)
			{
				result = 2;
			}
			else
			{
				result = info->cProbesXmit > 0;
			}
		}
	}

	return result;
}

// @retail 0x7b7b0
bool qos_target_result(long handle, s_qos_result *result, long index)
{
	bool success = false;

	if (g_4cf8d4 && handle != NONE)
	{
		s_qos_handle *element = qos_handle_get(handle);

		if (element)
		{
			XNQOSINFO *info = &element->qos->axnqosinfo[index];

			if ((info->bFlags & XNET_XNQOSINFO_TARGET_CONTACTED) && (info->bFlags & (XNET_XNQOSINFO_COMPLETE | XNET_XNQOSINFO_PARTIAL_COMPLETE)))
			{
				memset(result, 0, sizeof(*result));

				result->probes_sent = info->cProbesXmit;
				result->probes_received = info->cProbesRecv;
				result->rtt_minimum = info->wRttMinInMsecs;
				result->rtt_median = info->wRttMedInMsecs;
				result->upstream_bits_per_second = info->dwUpBitsPerSec;
				result->downstream_bits_per_second = info->dwDnBitsPerSec;
				if (info->bFlags & XNET_XNQOSINFO_DATA_RECEIVED)
				{
					result->data_size = info->cbData;
					result->data = info->pbData;
				}
				success = true;
			}
		}
	}

	return success;
}

// @retail 0x7b880
bool session_description_valid(const s_session_description *description)
{
	if (!description)
	{
		return false;
	}
	if (description->field2 < 0 || description->field2 >= 2)
	{
		return false;
	}
	if (description->field4 < 0 || description->field4 >= 5)
	{
		return false;
	}
	if (description->field8 != -1 && (description->field8 <= 0 || description->field8 > 0xfffe))
	{
		return false;
	}
	if (description->fieldc != -1 && (description->fieldc <= 0 || description->field8 > 0xfffe))
	{
		return false;
	}
	if (description->field10 < 0 || description->field10 >= 3)
	{
		return false;
	}
	if (description->field12 < 0 || description->field12 >= 3)
	{
		return false;
	}
	if (description->field14 < 0 || description->field14 >= 3)
	{
		return false;
	}
	if (description->field94 < 0 || description->field94 > 16)
	{
		return false;
	}
	if (description->field96 < 0 || description->field96 > 16)
	{
		return false;
	}
	if (description->field98 < 0 || description->field98 > 16)
	{
		return false;
	}
	if (description->field9a < 0 || description->field9a > 16)
	{
		return false;
	}
	if (description->field9c < 0 || description->field9c >= 5)
	{
		return false;
	}
	if (description->field9e < 0 || description->field9e >= 10)
	{
		return false;
	}
	if (description->fieldb4 < 0 || description->fieldb4 >= 3)
	{
		return false;
	}
	if (description->fieldb8 < 0)
	{
		return false;
	}
	if (description->fieldbc < 0 || description->fieldbc > 16)
	{
		return false;
	}
	if (description->field6e0 & 0xffffff00)
	{
		return false;
	}
	if (description->fielda8 != -1 && (description->fielda8 < 0 || description->fielda8 >= 4))
	{
		return false;
	}

	return true;
}

void utf8_string_to_utf16_string(const char *source, word *destination, long destination_count);
bool function_7efa0(s_bitstream *stream, s_player_appearance *appearance);

struct s_description_identity
{
	dword words[3];
};

// @retail 0x7c110
bool __stdcall function_07c110(s_bitstream *stream, void *session)
{
	s_session_description *description = (s_session_description *)session;
	byte *data = (byte *)session;
	*(word *)data = (word)function_1959c0(stream, 8);
	description->field2 = (short)function_1959c0(stream, 2);
	description->field4 = function_1959c0(stream, 3);
	description->field8 = function_1959c0(stream, 16) - 1;
	description->fieldc = function_1959c0(stream, 16) - 1;
	description->field10 = (short)function_1959c0(stream, 2);
	description->field12 = (short)function_1959c0(stream, 2);
	description->field14 = (short)function_1959c0(stream, 2);
	char name[32];
	function_195820(stream, name, 256);
	utf8_string_to_utf16_string(name, (word *)(data + 0x18), 32);
	function_195820(stream, data + 0x58, 64);
	function_195820(stream, data + 0x60, 128);
	function_195820(stream, data + 0x70, 288);
	description->field94 = (short)function_1959c0(stream, 5);
	description->field96 = (short)function_1959c0(stream, 5);
	description->field98 = (short)function_1959c0(stream, 5);
	description->field9a = (short)function_1959c0(stream, 5);
	description->field9c = (short)function_1959c0(stream, 3);
	description->field9e = (short)function_1959c0(stream, 4);
	*(long *)(data + 0xa0) = function_1959c0(stream, 4);
	*(long *)(data + 0xa4) = function_1959c0(stream, 32);
	description->fielda8 = function_1959c0(stream, 3) - 1;
	*(long *)(data + 0xac) = function_1959c0(stream, 32);
	*(bool *)(data + 0xb0) = function_1957d0(stream);
	description->fieldb4 = function_1959c0(stream, 2);
	description->fieldb8 = function_1959c0(stream, 32);
	description->fieldbc = (short)function_1959c0(stream, 5);
	bool valid = description->fieldbc >= 0 && description->fieldbc <= 16;
	long count = function_1959c0(stream, 5);
	valid = valid && count >= description->fieldbc && count <= 16;
	if (count < description->fieldbc) count = description->fieldbc;
	else if (count > 16) count = 16;
	for (long i = 0; valid && i < count; i++)
	{
		s_description_identity identity;
		char local_392e35[32];
		s_player_appearance appearance;
		function_195820(stream, &identity, 96);
		function_195820(stream, local_392e35, 256);
		long value = function_1959c0(stream, 32);
		long index = function_1959c0(stream, 5);
		function_7efa0(stream, &appearance);
		if (i < description->fieldbc)
		{
			*(s_description_identity *)(data + 0xbe + i * 12) = identity;
			utf8_string_to_utf16_string(local_392e35, (word *)(data + 0x17e + i * 64), 32);
			*(long *)(data + 0x580 + i * 4) = value;
			*(short *)(data + 0x5c0 + i * 2) = (short)(index - 1);
			*(s_player_appearance *)(data + 0x5e0 + i * 16) = appearance;
		}
	}
	description->field6e0 = function_1959c0(stream, 8);
	dword mask = function_1959c0(stream, 8);
	for (long j = 0; j < 8; j++)
	{
		if (mask & (1 << j))
		{
			long value = function_1959c0(stream, 32);
			if (description->field6e0 & (1 << j))
				*(long *)(data + 0x6e4 + j * 4) = value;
		}
	}
	*(bool *)(data + 0x704) = stream_read_bit(stream);
	if (*(bool *)(data + 0x704))
		function_195820(stream, data + 0x705, 96);
	else
		memset(data + 0x705, 0, 12);
	if (valid && !stream_overflowed(stream) && session_description_valid(description))
		return true;
	return false;
}


void utf16_string_to_utf8_string(const word *source, char *destination, long destination_count);
void function_7ee10(s_bitstream *stream, const s_player_appearance *appearance);

// @retail 0x7ba10
void __stdcall function_07ba10(s_bitstream *stream, void *session)
{
	const s_session_description *description = (const s_session_description *)session;
	const byte *data = (const byte *)session;
	char name[32];
	utf16_string_to_utf8_string((const word *)(data + 0x18), name, 32);
	stream_write_checked(stream, *(const short *)data, 8);
	stream_write_checked(stream, description->field2, 2);
	stream_write_checked(stream, description->field4, 3);
	stream_write_checked(stream, description->field8 + 1, 16);
	stream_write_checked(stream, description->fieldc + 1, 16);
	stream_write_checked(stream, description->field10, 2);
	stream_write_checked(stream, description->field12, 2);
	stream_write_checked(stream, description->field14, 2);
	function_1955d0(stream, name, 256);
	function_1955d0(stream, data + 0x58, 64);
	function_1955d0(stream, data + 0x60, 128);
	function_1955d0(stream, data + 0x70, 288);
	stream_write_checked(stream, description->field94, 5);
	stream_write_checked(stream, description->field96, 5);
	stream_write_checked(stream, description->field98, 5);
	stream_write_checked(stream, description->field9a, 5);
	stream_write_checked(stream, description->field9c, 3);
	stream_write_checked(stream, description->field9e, 4);
	stream_write_checked(stream, *(const long *)(data + 0xa0), 4);
	function_195720(stream, *(const dword *)(data + 0xa4), 32);
	stream_write_checked(stream, description->fielda8 + 1, 3);
	function_195720(stream, *(const dword *)(data + 0xac), 32);
	stream_write_bit(stream, *(const bool *)(data + 0xb0));
	stream_write_checked(stream, description->fieldb4, 2);
	function_195720(stream, description->fieldb8, 32);
	stream_write_checked(stream, description->fieldbc, 5);
	long count = description->fieldbc;
	stream_write_checked(stream, count, 5);
	for (long i = 0; i < count; i++)
	{
		s_description_identity identity;
		s_player_appearance appearance;
		char local_392e35[32];
		long value = 0;
		short index = NONE;
		if (i < description->fieldbc)
		{
			identity = *(const s_description_identity *)(data + 0xbe + i * 12);
			memcpy(local_392e35, data + 0x17e + i * 64, sizeof(local_392e35));
			utf16_string_to_utf8_string((const word *)(data + 0x17e + i * 64), local_392e35, 32);
			value = *(const long *)(data + 0x580 + i * 4);
			index = *(const short *)(data + 0x5c0 + i * 2);
			appearance = *(const s_player_appearance *)(data + 0x5e0 + i * 16);
		}
		else
		{
			memset(&identity, 0, sizeof(identity));
			memset(local_392e35, 0, sizeof(local_392e35));
			memset(&appearance, 0, sizeof(appearance));
		}
		function_1955d0(stream, &identity, 96);
		function_1955d0(stream, local_392e35, 256);
		function_195720(stream, value, 32);
		stream_write_checked(stream, index + 1, 5);
		function_7ee10(stream, &appearance);
	}
	stream_write_checked(stream, description->field6e0, 8);
	dword mask = description->field6e0;
	stream_write_checked(stream, mask, 8);
	for (long j = 0; j < 8; j++)
	{
		dword value = 0;
		if (description->field6e0 & (1 << j)) value = *(const dword *)(data + 0x6e4 + j * 4);
		if (mask & (1 << j)) function_195720(stream, value, 32);
	}
	stream_write_bit(stream, *(const bool *)(data + 0x704));
	if (*(const bool *)(data + 0x704)) function_1955d0(stream, data + 0x705, 96);
}

void function_1947a0(s_bitstream *stream);

// @retail 0x7c530
bool function_7c530(const byte *data, long size, void *description)
{
	bool result = false;
	if (size > 0 && data)
	{
		s_bitstream stream;
		stream.unknown08 = 1;
		stream.data = (byte *)data;
		stream.size_in_bytes = size;
		stream.mode = 0;
		stream.bit_position = 0;
		stream.checkpoint_count = 0;
		stream.error = false;
		function_1947a0(&stream);
		result = function_07c110(&stream, description);
		if (result && (stream.bit_position > (stream.size_in_bytes << 3) || stream.error))
			result = false;
	}
	return result;
}
