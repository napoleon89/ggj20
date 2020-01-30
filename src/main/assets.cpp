#define STB_IMAGE_IMPLEMENTATION
#include <core/stb_image.h>
#include <core/memory.h>
#include <engine/config.h>

#include <nanovg/nanovg.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <core/stb_image_write.h>

#include <game/assets.h>
Texture2D loadTexture(u8 *image_data, u64 image_size, RenderContext::Format format, int force_components) {
	int tex_w, tex_h, tex_comps;
	// stbi_set_flip_vertically_on_load(true);
	u8 *tex_data = (u8 *)stbi_load_from_memory((stbi_uc *)image_data, image_size, &tex_w, &tex_h, &tex_comps, force_components);
	const char *fail_reason = stbi_failure_reason();
	
	Texture2D result  = RenderContext::createTexture2D(tex_data, (u32)tex_w,  (u32)tex_h, format);
	stbi_image_free(tex_data);
	return result;	
}

Texture2D loadTextureFromFile(const char *filename, const std::string& extension, RenderContext::Format format, int force_components) {
	const std::string path = std::string(filename) + "." + extension;
	FileData texture_file = Platform::readEntireFile(path.c_str());
	
	Texture2D result = loadTexture((u8 *)texture_file.contents, texture_file.size, format, force_components);
	
	Platform::free(texture_file.contents);
	return result;
}

Shader loadShaderFromFile(std::string shader_name) {
	std::string v_path = "data/shaders/" + shader_name + ".vert";
	std::string p_path = "data/shaders/" + shader_name + ".frag";	

	FileData vertex_file = Platform::readEntireFile(v_path.c_str());
	FileData pixel_file = Platform::readEntireFile(p_path.c_str());

	Shader result = RenderContext::createShader(
		shader_name, 
		vertex_file.contents, 
		vertex_file.size, 
		pixel_file.contents, 
		pixel_file.size
	);
	return result;
}

SpriteAnimationGroup loadSpriteAnimationGroupFromFile(const std::string& anim_path, const std::string& group_name) {
	std::string path = anim_path + "/";
	ConfigFile anim_cfg = {};
	anim_cfg.parseFromFile((path + group_name + ".cfg").c_str());

	SpriteAnimation common_data = {};
	anim_cfg.fillStructFromMemory(&common_data);

	SpriteAnimationGroup group = {};
	for(u32 i = 0; i < (u32)Direction::Count; i++) {
		group.animations[i] = common_data;
	}

	group.animations[(u32)Direction::Down].sheet = loadTextureFromFile((path + group_name + "_FRONT").c_str());
	group.animations[(u32)Direction::Up].sheet = loadTextureFromFile((path + group_name + "_BACK").c_str());
	group.animations[(u32)Direction::DiagUp].sheet = loadTextureFromFile((path + group_name + "_DIAG_B").c_str());
	group.animations[(u32)Direction::DiagDown].sheet = loadTextureFromFile((path + group_name + "_DIAG_F").c_str());
	group.animations[(u32)Direction::Side].sheet = loadTextureFromFile((path + group_name + "_SIDE").c_str());
	
	return group;
}

SpriteAnimation loadSpriteAnimationFromFile(const std::string& anim_path) {
	ConfigFile anim_cfg = {};
	anim_cfg.parseFromFile((anim_path + ".cfg").c_str());

	SpriteAnimation result = {};
	anim_cfg.fillStructFromMemory(&result);
	result.sheet = loadTextureFromFile(anim_path.c_str());
	return result;
}