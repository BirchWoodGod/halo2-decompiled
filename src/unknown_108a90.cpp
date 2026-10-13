// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_108A90.CPP: per-object-type event dispatch; walks the null-terminated
   list of handler tables of an object's type definition */

#include "unknown_11c920.h"
#include "globals.h"

struct s_object_handlers
{
	byte unknown00[0x2c];
	bool (__stdcall *handler2c)(long, long, long);
	void (__stdcall *handler30)(long, long);
	byte unknown34[4];
	void (__stdcall *handler38)(long);
	void (__stdcall *handler3c)(long);
	bool (__stdcall *handler40)(long);
	void (__stdcall *handler44)(long);
	bool (__stdcall *handler48)(long, long, long);
	bool (__stdcall *handler4c)(long, long, real *, bool *);
	void (__stdcall *handler50)(long);
	void (__stdcall *handler54)(long);
	void (__stdcall *handler58)(long, long);
	void (__stdcall *handler5c)(long, long);
};

struct s_object_type_definition
{
	byte unknown00[0x84];
	s_object_handlers *handlers[1];
};

struct s_object
{
	byte unknown00[0xaa];
	signed char type;
};

struct s_object_header
{
	byte unknown00[8];
	s_object *object;
};


#define OBJECT_TYPE_DEFINITION(index) (g_468630[((s_object_header *)g_4e0300->data)[(index) & 0xFFFF].object->type])

// @retail 0x108a90
bool function_108a90(long object_index, long a, long b)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	bool result = true;
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler2c)
		{
			result = result && handlers->handler2c(object_index, a, b);
		}
		i++;
		slot = &definition->handlers[i];
	}
	return result;
}

// @retail 0x108b10
void function_108b10(long object_index, long a)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler30)
		{
			handlers->handler30(object_index, a);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108b80
void function_108b80(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler38)
		{
			handlers->handler38(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108bf0
void function_108bf0(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler3c)
		{
			handlers->handler3c(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108c60
bool function_108c60(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	bool result = false;
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler40)
		{
			result |= handlers->handler40(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
	return result;
}

// @retail 0x108cd0
void function_108cd0(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	long i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler44)
		{
			handlers->handler44(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108d30
bool function_108d30(long object_index, long a, long b)
{
	struct { s_object_type_definition *definition; bool result; s_object_handlers **list; } iterator;
	iterator.definition = OBJECT_TYPE_DEFINITION(object_index);
	iterator.result = false;
	iterator.list = iterator.definition->handlers;
	while (!iterator.result && *iterator.list)
	{
		s_object_handlers *handlers = *iterator.list;
		if (handlers->handler48)
		{
			iterator.result = handlers->handler48(object_index, a, b);
		}
		iterator.list++;
	}
	return iterator.result;
}

// @retail 0x108d90
bool function_108d90(long object_index, long a, real *out_real, bool *out_bool)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	bool result = false;
	*out_real = 0.0f;
	*out_bool = false;
	s_object_handlers **list = definition->handlers;
	while (!result && *list)
	{
		s_object_handlers *handlers = *list;
		if (handlers->handler4c)
		{
			result = handlers->handler4c(object_index, a, out_real, out_bool);
		}
		list++;
	}
	if (!result)
	{
		*out_real = 0.0f;
		*out_bool = result;
	}
	return result;
}

// @retail 0x108e10
void function_108e10(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler50)
		{
			handlers->handler50(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108e80
void function_108e80(long object_index)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler54)
		{
			handlers->handler54(object_index);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108ef0
void function_108ef0(long object_index, long a)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler58)
		{
			handlers->handler58(object_index, a);
		}
		i++;
		slot = &definition->handlers[i];
	}
}

// @retail 0x108f60
void function_108f60(long object_index, long a)
{
	s_object_type_definition *definition = OBJECT_TYPE_DEFINITION(object_index);
	short i = 0;
	s_object_handlers **slot = &definition->handlers[0];
	while (definition->handlers[i])
	{
		s_object_handlers *handlers = *slot;
		if (handlers->handler5c)
		{
			handlers->handler5c(object_index, a);
		}
		i++;
		slot = &definition->handlers[i];
	}
}
