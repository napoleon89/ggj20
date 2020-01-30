#include <core/utils/args.h>
#include <core/utils/string_utils.h>

Dictionary<std::string, std::string> ArgsUtil::parse(char *args[], int arg_count) {
	Dictionary<std::string, std::string> result;

	for(int i = 0; i < arg_count; i++) {
		const std::string arg = args[i];

		if (arg.size() < 2) continue;
		if (arg[0] != '-' || arg[1] != '-') continue;
		
		s32 split_index = arg.find(":");
		if (split_index == std::string::npos) continue;


		
		std::string key = StringUtils::trimStart(StringUtils::subString(arg, 0, split_index+1), "--");
		if (key.empty()) continue;
		
		std::string value = StringUtils::subString(arg, split_index + 1, arg.size());
		if (key.empty()) continue;
		
		result.insertItem(key, value);
	}
	
	return result;
}
