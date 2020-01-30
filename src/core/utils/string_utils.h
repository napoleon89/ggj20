#pragma once

#include <string>
#include <core/collections/dynamic_array.h>

namespace StringUtils {
	const DynamicArray<std::string> split(const std::string &input, const std::string &split_str);
	const std::string removeExtension(const std::string input);
	const std::string subString(const std::string &input, const u32 start, const u32 end);
	const std::string trimStart(const std::string &input, const std::string &trim_str);
}
