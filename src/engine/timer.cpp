#include <engine/timer.h>
#include <core/platform.h>

Timer::Timer() { 
	if(perf_count_freq == 0) 
		perf_count_freq = Platform::getPerformanceFrequency(); 
}

u64 Timer::start() { 
	start_time = getWallClock(); 
	return start_time; 
}

u64 Timer::getWallClock() { 
	return Platform::getPerformanceCounter(); 
}

f32 Timer::getSecondsElapsed() { 
	f32 result = ((f32)(getWallClock() - start_time) / (f32)perf_count_freq); 
	return result; 
}

f32 Timer::getMillisecondsElapsed() { 
	return getSecondsElapsed() * 1000.0f; 
}

f32 Timer::getStartTimeMilliseconds() {
	f32 result = ((f32)(start_time) / (f32)perf_count_freq);
	return result * 1000.0f;
}


u64 Timer::perf_count_freq = 0;
