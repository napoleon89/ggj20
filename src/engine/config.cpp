#include <engine/config.h>
#include <core/platform.h>
#include <core/utils/string_utils.h>


const bool isNewLineChar(const char value) {
	return value == '\r' || value == '\n';
}

void ConfigFile::parseFromFile(const char *filename) {
	FileData config_file = Platform::readEntireFile(filename);
	parseFromMemory((u8 *)config_file.contents, config_file.size);	
	Platform::free(config_file.contents);
}

struct ConfigVariable {
	std::string name;
	std::string value;
};

void ConfigFile::parseFromMemory(u8 *contents, const u64 size) {
	DynamicArray<ConfigVariable> variables;

	u8 *walker = contents;
	while (walker < contents + size) {
		const u8 *line_start = walker;
		char current_char;
		u32 line_length = 0;
		while ((current_char = *walker++) && !isNewLineChar(current_char)) {
			line_length++;
		}
		
		if (line_length > 0) {
			const std::string line = std::string(line_start, line_start + line_length);
			switch (line[0]) {
				case ';': break; // comment

				case '[': { // title
					u32 title_length = 0;
					for (u32 i = 1; i < line_length; i++) {
						if (line[i] == ']') {
							title_length = i;
						}
					}
					if (title_length == 0) {
						Log::error("Error: Failed to parse title '%s' on line %d\n", line.c_str(), 0);
						return;
					}
					const std::string title = std::string(line_start + 1, line_start + line_length - 1);
					// printf("Title: %s\n", title.c_str());
				} break;

				default: { // variable
					u32 variable_length = 0;
					for (u32 i = 1; i < line_length; i++) {
						if (line[i] == ' ' || line[i] == '=') {
							variable_length = i;
							break;
						}
					}
					u32 equal_index = 0;
					for (u32 i = variable_length; i < line_length; i++) {
						if (line[i] == '=') {
							equal_index = i;
							break;
						}
					}
					if (variable_length == 0 || equal_index == 0) {
						Log::error("Error: Variable %s is never assigned to.\n", line.c_str());
						return;
					}
					const std::string variable_name = std::string(line_start, line_start + variable_length);


					u32 value_index = 0;
					for (u32 i = equal_index; i < line_length; i++) {
						if (line[i] != ' ' && line[i] != '=') {
							value_index = i;
							break;
						}
					}

					u32 new_line_length = line_length;
					if (*(line_start + value_index) == '\"' && *(line_start + (line_length-1)) == '\"') {
						value_index += 1;
						new_line_length -= 1;
					}

					const std::string variable_value = std::string(line_start + value_index, line_start + new_line_length);
					// printf("%s: %s\n", variable_name.c_str(), variable_value.c_str());
					// ConfigEntry value_entry = {};
					// value_entry.value = variable_value;

					variables.add({ variable_name, variable_value });
				} break;
			}
		}
	}

	for(const ConfigVariable &variable : variables) {
		DynamicArray<std::string> scopes = StringUtils::split(variable.name, ".");
		ConfigEntry *current_entry = this;

		for (u32 i = 0; i < scopes.count; i++) {
			const std::string &scope = scopes.getRefConst(i);
			ConfigEntry *new_entry = current_entry->entries.search(scope);
			
			if (new_entry == nullptr) {
				new_entry = current_entry->entries.insertItem(scope, ConfigEntry());
			}
			current_entry = new_entry;
		}
		
		current_entry->value = variable.value;
	}
}

void ConfigFile::fillStructFromSettingsMap(const ConfigEntry &entry, Reflection::Class &struct_to_fill, u8 *parent_instance) {
	for (const auto &pair : entry.entries) {
		const std::string variable_name = pair.key;
		Reflection::Field *field = struct_to_fill.findField(variable_name.c_str());

		if (field != nullptr) {

			if (field->type->is_primitive) {
				const char *new_value = pair.value.value.c_str();
				if (field->isStatic()) {
					field->setStaticValueFromString(new_value);
				} else {
					field->setValueFromString(parent_instance, new_value);
				}
			} else {
				u8 *new_address = 0;
				if (field->isStatic())
					new_address = (u8 *)field->static_address;
				else
					new_address = parent_instance + field->offset;
				fillStructFromSettingsMap(pair.value, *(Reflection::Class *)field->type, new_address);
			}
		}
	}
}

void ConfigFile::openForWriting(const std::string &filename) {
	file_handle = Platform::openFileForWriting(filename.c_str());
}

void ConfigFile::close() {
	Platform::closeOpenFile(file_handle);
}

void ConfigFile::writeTitle(const std::string &title) {
	Platform::writeLineToFile(file_handle, "[" + title + "]");
}

void ConfigFile::writeComment(const std::string &comment) {
	Platform::writeLineToFile(file_handle, "; " + comment);
}

void ConfigFile::writeVariable(const std::string &name, const std::string &value) {
	writeVariableInternal(name, "\"" + value + "\"");
}

void ConfigFile::writeVariable(const std::string &name, const std::string &&value) {
	writeVariableInternal(name, "\"" + value + "\"");
}

void ConfigFile::writeVariable(const std::string &name, const u32 value) {
	writeVariableInternal(name, std::to_string(value));
}

void ConfigFile::writeVariable(const std::string &name, const u64 value) {
	writeVariableInternal(name, std::to_string(value));
}

void ConfigFile::writeVariable(const std::string &name, const s32 value) {
	writeVariableInternal(name, std::to_string(value));
}

void ConfigFile::writeVariable(const std::string &name, const f32 value) {
	writeVariableInternal(name, std::to_string(value));
}

void ConfigFile::writeVariableInternal(const std::string &name, const std::string &&value) {
	Platform::writeLineToFile(file_handle, name + " = " + value);
}

