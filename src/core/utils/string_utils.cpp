#include <core/utils/string_utils.h>

const DynamicArray<std::string> StringUtils::split(const std::string &input, const std::string &split_str) {
	DynamicArray<std::string> result;

	u32 start_index = 0;
	size_t find_index = input.find(split_str, start_index);
	while(find_index != std::string::npos) {
		result.add(subString(input, start_index, find_index));
		start_index = find_index += split_str.size();
		find_index = input.find(split_str, start_index);
	}

	result.add(subString(input, start_index, input.size()));
	
	return result;
}

const std::string StringUtils::removeExtension(const std::string input) {
	DynamicArray<std::string> elements = split(input, ".");
	return elements.getRefConst(0);;
}

const std::string StringUtils::subString(const std::string &input, const u32 start, const u32 end) {
	return std::string(input.begin() + start, input.begin() + end);
}

const std::string StringUtils::trimStart(const std::string &input, const std::string &trim_str) {

	u32 start_index = 0;
	s32 find_index = input.find(trim_str, start_index);

	while(find_index == start_index && find_index != std::string::npos) {
		start_index += trim_str.size();
		find_index = input.find(trim_str, start_index);
	}

	return subString(input, start_index, input.size() - 1);
}
