#include <functional>

#include <core/utils/hashing.h>
#include <core/math.h>

static std::hash<std::string> g_string_hash;

u32 Hashing::computeHash(const std::string &key, const u32 a, const u32 bucket_count) {
	u64 hash = 0;
	const u32 string_length = key.size();
	for (u32 i = 0; i < string_length; i++) {
		u64 val = Math::pow(a, string_length - (i + 1));
		hash += val * (unsigned long)key[i];
		hash = hash % bucket_count;
	}
	return (u32)hash;
}

u32 Hashing::getHashindex(const std::string &key, const u32 num_buckets, const u32 attempt) {
	u32 hash_a = computeHash(key, HASH_PRIME_VALUE_1, num_buckets);
	u32 hash_b = computeHash(key, HASH_PRIME_VALUE_2, num_buckets);
	if(hash_b == num_buckets-1) hash_b++;
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

