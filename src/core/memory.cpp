#include "memory.h"

#include <core/platform.h>
#include <core/collections/dictionary.h>

global_variable Dictionary<std::string, void*> g_allocations;

void* Memory::allocInternal(u64 size, const char* function, const char* file, const char* line) {
	return Platform::alloc(size);
}

void Memory::free(void* data) {
	Platform::free(data);
}
