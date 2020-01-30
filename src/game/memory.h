#pragma once

#include <core/std.h>
struct MemoryBlock {
	 void *memory;
	u64 size;
};

#define MEMORY_STORE_COUNT 4

struct MemoryStore {
	void *memory;
	union {
		struct {
			MemoryBlock game_memory;
			MemoryBlock asset_memory;
			MemoryBlock frame_memory;
			MemoryBlock debug_memory;
			
		};
		MemoryBlock blocks[MEMORY_STORE_COUNT];
	};
	
	inline u64 calcTotalSize() {
		u64 mem_total_size = 0;
		for(int i = 0; i < MEMORY_STORE_COUNT; i++) {
			mem_total_size += blocks[i].size;
		}
		return mem_total_size;
	}
};

