#pragma once

#include <core/std.h>
#include <core/collections/dictionary.h>
#include <string>
#include <core/platform.h>
#include <engine/reflection.h>

#define ConfigDict Dictionary<std::string, ConfigEntry>

struct ConfigEntry {
	ConfigDict entries;
	std::string value;
};


/*
	config format
	[this is a title]
	; this is a comment
	this_is_a_string_variable = "value"
	this_is_a_number_variable = 100.5
 */

struct ConfigFile : public ConfigEntry {
	void *file_handle = nullptr; 
	void parseFromFile(const char *filename);
	void parseFromMemory(u8 *contents, const u64 size);

	template<typename T>
	void fillStaticStructFromMemory();

	template<typename T>
	void fillStructFromMemory(T* instance);

	void openForWriting(const std::string &filename);
	void close();
	void writeTitle(const std::string &title);
	void writeComment(const std::string &comment);
	void writeVariable(const std::string &name, const std::string &&value);
	void writeVariable(const std::string &name, const std::string &value);
	void writeVariable(const std::string &name, const u32 value);
	void writeVariable(const std::string &name, const u64 value);
	void writeVariable(const std::string &name, const s32 value);
	void writeVariable(const std::string &name, const f32 value);

private:
	void writeVariableInternal(const std::string &name, const std::string &&value);
	void fillStructFromSettingsMap(const ConfigEntry &entry, Reflection::Class &struct_to_fill, u8 *parent_instance);
};

template<typename T>
void ConfigFile::fillStaticStructFromMemory() {
	Reflection::Class *struct_info = Reflection::getStruct<T>();
	fillStructFromSettingsMap(*this, *struct_info, (u8 *)struct_info->fields[0].static_address);
}

template<typename T>
void ConfigFile::fillStructFromMemory(T* instance) {
	Reflection::Class *struct_info = Reflection::getStruct<T>();
	fillStructFromSettingsMap(*this, *struct_info, (u8 *)instance);
}