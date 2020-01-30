#include <stdio.h>
#include <GL/glew.h>
#include <core/std.h>
#include <core/logger.h>
#include <core/platform.h>
#include <core/collections/dictionary.h>
#include <core/collections/dynamic_array.h>
#include <core/utils/string_utils.h>
#include <engine/config.h>
#include <engine/timer.h>

bool isIgnorableChar(const char c) {
	return c == '\r' || c == '\n' || c == ' ';
}

struct Scope {
	u32 start_index;
	u32 end_index;
	std::string contents;
};

Scope findScope(const std::string& preface, const std::string& shader) {
	Scope result = {};
	
	u32 start = shader.find(preface);

	result.start_index = start;
	
	u32 scope_start = 0;
	for(u32 i = start+preface.size(); i < shader.size(); i++) {
		if(shader[i] == '{') {
			scope_start = i+1;
			break;
		}	
	}
	s32 scope_depth = 1;
	u32 scope_end = 0; 
	for(u32 i = scope_start+1; i < shader.size(); i++) {
		if(shader[i] == '{') {
			scope_depth++;
		}

		if(shader[i] == '}') {
			scope_depth--;
		}

		if(scope_depth == 0) {
			scope_end = i;
			break;
		}
	}

	result.end_index = scope_end+1;

	result.contents = StringUtils::subString(shader, scope_start, scope_end);
	return result;
}

DynamicArray<std::string> findLinesInScope(const Scope& scope) {
	DynamicArray<std::string> lines;

	bool reading_line = false;
	u32 line_start = 0;
	u32 line_end = 0;
	for(u32 i = 0; i < scope.contents.size(); i++) {
		const char c = scope.contents[i];
		if(!reading_line && isIgnorableChar(c)) continue;

		if(reading_line && c == ';') {
			line_end = i+1;
			reading_line = false;
			lines.add(StringUtils::subString(scope.contents, line_start, line_end));
			continue;
		}
		
		if(!reading_line && isalpha(c)) {
			line_start = i;
			reading_line = true;
		}
	}
	return lines;
}

#ifdef __linux__
#define _popen popen
#define _pclose pclose
#define _strtoui64 atll
#endif

std::string exec(std::string command) {
   char buffer[128];
   std::string result = "";

   // Open pipe to file
   FILE* pipe = _popen(command.c_str(), "rt");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL) {
        result += std::string(buffer);
	  }
   }

   _pclose(pipe);
   return result;
}

int main(int arg_count, char *args[]) {
	if(!Platform::init()) {
		Log::error("Failed to initialize platform");
		return -1;
	}

	if (arg_count < 4) {
		Log::error("Please specify a working directory, an output directory and a temp directory");
		return -1;
	}

	std::string base_path = Platform::getBasePath();
	
	const std::string working_dir = args[1];
	const std::string output_dir = args[2];
	const std::string temp_dir = args[3];

	const DynamicArray<FileInfo> &raw_shader_files = Platform::getDirectoryContents(working_dir, "*.glsl");
	
	enum class ScopeType {
		VertIn,
		Vary,
		VertFunc,
		FragFunc,
		Count,
	};

	const std::string scope_names[] = {
		"VertIn",
		"Vary",
		"void vert()",
		"void frag()",
	};

	Scope scopes[(u32)ScopeType::Count];

	
	ConfigFile cache;
	cache.parseFromFile((temp_dir + "/shaders.db").c_str());
	cache.openForWriting((temp_dir + "/shaders.db").c_str());
	u32 skipped = 0;
	
	for(u32 i = 0; i < raw_shader_files.count; i++) {
		const FileInfo& info = raw_shader_files.getRefConst(i);
		std::string shader_name = StringUtils::removeExtension(info.filename);

		
		
		ConfigEntry* cache_entry = cache.entries.search(shader_name);
		
		if(cache_entry != nullptr) {
			std::string value = cache_entry->value;
			u64 time = atoll(value.c_str());
			if(info.last_write_time.getAsU64() <= time) {
				skipped++;
				cache.writeVariable(shader_name, info.last_write_time.getAsU64());
				continue;
			}
		}
		
		FileData data = Platform::readEntireFile(info.full_path.c_str());
		std::string shader_src = std::string(data.contents, data.size);
		
		
		std::string result = shader_src;

		for(u32 i = 0; i < (u32)ScopeType::Count; i++) {
			scopes[i] = findScope(scope_names[i], result);
			result.replace(result.begin() + scopes[i].start_index, result.begin() + scopes[i].end_index, "");
		}

		const DynamicArray<std::string> vertex_inputs = findLinesInScope(scopes[(u32)ScopeType::VertIn]);
		
		std::string vertex_input_str = "";
		for(u32 i = 0; i < vertex_inputs.count; i++) {
			const std::string& input = vertex_inputs.getRefConst(i);
			vertex_input_str += "layout(location = " + std::to_string(i) + ") in " + input + "\r\n";
		}

		const DynamicArray<std::string> varyings = findLinesInScope(scopes[(u32)ScopeType::Vary]);
		
		std::string varying_str_vert = "";
		for(u32 i = 0; i < varyings.count; i++) {
			const std::string& input = varyings.getRefConst(i);
			varying_str_vert += "out " + input + "\r\n";
		}

		std::string varying_str_frag = "";
		for(u32 i = 0; i < varyings.count; i++) {
			const std::string& input = varyings.getRefConst(i);
			varying_str_frag += "in " + input + "\r\n";
		}

		const std::string version_str = "#version 450 core\r\n";

		std::string final_vert = version_str;
		final_vert += vertex_input_str;
		final_vert += "\r\n";
		final_vert += varying_str_vert;
		final_vert += result;
		final_vert += "void main() {\r\n";
		final_vert += scopes[(u32)ScopeType::VertFunc].contents;
		final_vert += "}";

		std::string final_frag = version_str;
		final_frag += varying_str_frag;
		final_frag += result;
		final_frag += "out vec4 out_color; \r\n";
		final_frag += "void main() {\r\n";
		final_frag += scopes[(u32)ScopeType::FragFunc].contents;
		final_frag += "}";

		const std::string vert_filename = output_dir + "/" + shader_name + ".vert";
		const std::string frag_filename = output_dir + "/" + shader_name + ".frag";

		void* vert_handle = Platform::openFileForWriting(vert_filename.c_str());
		Platform::writeToFile(vert_handle, final_vert);
		Platform::closeOpenFile(vert_handle);

		void* frag_handle = Platform::openFileForWriting(frag_filename.c_str());
		Platform::writeToFile(frag_handle, final_frag);
		Platform::closeOpenFile(frag_handle);

		std::string command = "\"" + base_path + "glslangValidator.exe" + "\" " + vert_filename + "";
		std::string exe_result = exec(command);

		if(!exe_result.empty()) {
			Log::error("%s", exe_result.c_str());
			continue;
		}

		cache.writeVariable(shader_name, info.last_write_time.getAsU64());
		
		Log::success("Compiled %s", shader_name.c_str());
	}

	if(skipped > 0) {
		Log::success("Skipped %d shaders", skipped);
	}

	cache.close();
	
	Platform::uninit();
}