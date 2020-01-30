#pragma once

#include <core/std.h>

struct Memory {
	static void* allocInternal(u64 size, const char* function, const char* file, const char* line);
	static void free(void* data);
};


#define Memory_alloc(size) Memory::allocInternal(size, __FUNCTION__, __FILE__, "")
