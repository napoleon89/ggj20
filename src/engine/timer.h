#pragma once

#include <core/std.h>

class Timer
{
	private:
		u64 start_time;
		static u64 perf_count_freq;
		
	public:
		Timer();
		u64 start();
		u64 getWallClock();
		f32 getSecondsElapsed();
		f32 getMillisecondsElapsed();
		f32 getStartTimeMilliseconds();
};
