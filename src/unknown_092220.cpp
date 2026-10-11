// @flags /O2 /Gr
/* UNKNOWN_092220.CPP: Xbox Live online tasks that follow the team
   balancing (0x92220..0x927b0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include <xtl.h>
#include <xonline.h>

HRESULT online_task_continue(s_type_9df9da *task);
long online_task_new_if_logged_on(void);
void function_6b640(long task_index);

static inline s_type_9df9da *online_task_try_and_get(long task_index)
{
	s_type_9df9da *task = 0;
	if (task_index != NONE)
	{
		s_record_pool *data = g_4cf78c;
		long absolute_index = task_index & 0xffff;
		if (absolute_index < data->high_water_index)
		{
			s_type_9df9da *candidate = (s_type_9df9da *)(data->data + data->size * absolute_index);
			if (candidate->salt != 0 && candidate->salt == (task_index >> 16))
				task = candidate;
		}
	}
	return task;
}

/* online_task_new (src/online_tasks.cpp), which retail inlines here */
static inline long online_task_new_inline(void)
{
	long task_index = record_pool_allocate(g_4cf78c);
	if (task_index != NONE)
	{
		s_type_9df9da *task = (s_type_9df9da *)g_4cf78c->data + (task_index & 0xffff);
		task->handle = 0;
		task->type = NONE;
		task->controller_index = NONE;
		task->flags = 0;
	}
	return task_index;
}

/* starts reading the given statistics */
// @retail 0x92220
long online_stats_read(word count, XONLINE_STAT_SPEC *specs)
{
	for (long i = 0; i < count; i++)
	{
		if (specs[i].xuidUser.dwUserFlags == 0xbad00000 || (specs[i].xuidUser.dwUserFlags & 3))
			return NONE;
	}
	long task_index = NONE;
	if (online_logon_connected())
		task_index = online_task_new_inline();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlineStatRead(count, specs, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				return NONE;
			}
		}
	}
	return task_index;
}

/* retrieves the requested statistics */
// @retail 0x92330
bool online_stats_read_result(long task_index, XONLINE_STAT_SPEC *specs, word count, byte *extra_buffer, word extra_size)
{
	s_type_9df9da *task = online_task_try_and_get(task_index);
	bool result = false;
	if (task && online_logon_connected() && (task->flags & 6))
	{
		if (SUCCEEDED(XOnlineStatReadGetResult((XONLINETASK_HANDLE)task->handle, count, specs, extra_size, extra_buffer)))
			result = true;
	}
	return result;
}

/* starts writing the given statistics */
// @retail 0x923c0
long online_stats_write(XONLINE_STAT_SPEC const *specs, word count)
{
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			if (SUCCEEDED(XOnlineStatWrite(count, specs, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x13;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				return NONE;
			}
		}
	}
	return task_index;
}

// @retail 0x92450
bool online_stats_write_succeeded(long task_index)
{
	s_type_9df9da *task = online_task_try_and_get(task_index);
	bool result = false;
	if (task && online_logon_connected() && (task->flags & 6))
	{
		HANDLE result_handle;
		PXONLINE_STAT_ATTACHMENT_REFERENCE references;
		DWORD count;
		if (SUCCEEDED(XOnlineStatWriteGetResult((XONLINETASK_HANDLE)task->handle, &result_handle, &references, &count)))
			result = true;
	}
	return result;
}

// @retail 0x924e0
long online_round_register(bool free_for_all, XNKID const *session_id, ULONGLONG const *round_key, long seconds)
{
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			dword flags = (free_for_all ? 0x10 : 0) | 0x0c;
			if (SUCCEEDED(XOnlineArbitrationRegister(&id, seconds, flags, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
				return task_index;
			}
			else
			{
				function_6b640(task_index);
				return NONE;
			}
		}
	}
	return task_index;
}

/* submits the completed round's statistics */
// @retail 0x925c0
long online_round_report(XNKID const *session_id, ULONGLONG const *round_key, long count, XONLINE_STAT_PROC const *procedures, bool flag0, bool flag1)
{
	long task_index = NONE;
	for (long i = 0; i < count; i++)
	{
		XUID const *user = NULL;
		word kind = procedures[i].wProcedureID;
		if (kind == 0x8001 || kind == 0x8003 || kind == 0x8007)
			user = &procedures[i].Update.xuid;
		if (user->dwUserFlags == 0xbad00000 || (user->dwUserFlags & 3))
			goto done;
	}
	if (count <= 1000)
	{
		if (online_logon_connected())
			task_index = online_task_new_inline();
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			dword flags = (flag0 ? 1 : 0) | (flag1 ? 2 : 0);
			if (SUCCEEDED(XOnlineArbitrationReport(&id, flag1 ? 0 : count, procedures, NULL, flags, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				task_index = NONE;
			}
		}
	}
done:
	return task_index;
}

/* whether the task failed because the service is not available */
// @retail 0x92750
bool online_task_service_unavailable(long task_index)
{
	bool result = false;
	s_type_9df9da *task = online_task_try_and_get(task_index);
	if (online_task_continue(task) == 0x8015b108)
		result = true;
	return result;
}

// @retail 0x927b0
long online_round_extend(ULONGLONG const *round_key, XNKID const *session_id, word seconds)
{
	long task_index = online_task_new_if_logged_on();
	if (task_index != NONE)
	{
		s_type_9df9da *task = online_task_try_and_get(task_index);
		if (task)
		{
			XONLINE_ARB_ID id;
			id.SessionID = *session_id;
			id.qwRoundID = *round_key;
			if (SUCCEEDED(XOnlineArbitrationExtendRound(&id, seconds, NULL, (PXONLINETASK_HANDLE)&task->handle)))
			{
				task->flags = 1;
				task->type = 0x12;
				task->controller_index = NONE;
			}
			else
			{
				function_6b640(task_index);
				return NONE;
			}
		}
	}
	return task_index;
}
