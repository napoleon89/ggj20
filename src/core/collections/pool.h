#pragma once

#include <core/collections/buffer.h>

enum class PoolState {
	Inactive,
	Active
};

template<typename T>
struct Pool {
	Buffer<PoolState> states;
	Buffer<T> items;
};