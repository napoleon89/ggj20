#pragma once

#include <core/std.h>
#include <string>

namespace Hashing {
	inline static const u32 HASH_PRIME_VALUE_1 = 257;
	inline static const u32 HASH_PRIME_VALUE_2 = 271;
	u32 computeHash(const std::string &key, const u32 a, const u32 bucket_count);
	u32 getHashindex(const std::string &key, const u32 num_buckets, const u32 attempt);
}