#pragma once

#include <engine/timer.h>
#include <atomic>
#include <core/collections/dynamic_array.h>

struct LocalTimer {
	Timer timer;
	int profiler_entry_index;

	LocalTimer(const char *function_name);
	~LocalTimer();
};

struct ProfilerEntry {
	const char *function_name;
	f32 time_taken_ms;
	f32 start_time;
	u32 stack_depth;
};

struct ProfilerFrame {
	DynamicArray<ProfilerEntry> entries;
	u32 frame_index = 0;
	f32 total_time;
};

#define PROFILER_FRAME_BUFFER_COUNT 8

struct Profiler {
	inline static u32 stack_depth = 0;
	inline static ProfilerFrame frames[PROFILER_FRAME_BUFFER_COUNT];
	inline static int current_frame_index;
	inline static bool paused = false;
	inline static Timer frame_timer;

	static int startProfilingFunction(const char* function_name);
	static void finishProfilingFunction(int entry_index, f32 time_taken_ms);
	static void finishFrame();
	static void startFrame();
	static void editorGui(bool* open);
};

#define ProfileCombineNameEx(x, y) x##y
#define ProfileCombineName(x, y) ProfileCombineNameEx(x, y)
#define ProfileScope(name) LocalTimer ProfileCombineName(PROFILER_LOCAL_TIMER_, __LINE__) = LocalTimer(name)
#define ProfileFunc ProfileScope(__FUNCTION__)