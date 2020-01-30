#pragma once

#include <core/collections/dictionary.h>
#include <string>

namespace ArgsUtil {
	Dictionary<std::string, std::string> parse(char *args[], int arg_count);
};