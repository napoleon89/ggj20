#pragma once

#include <core/render_context.h>

#include <engine/animation2d.h>

struct Font {
	int font_id;
};

struct Assets {
	struct ShaderList {
		Shader imgui;
		Shader editor;
		Shader sprite;
		Shader gui;
		Shader text;
	};

	
	inline static ShaderList shaders;

	inline static Texture2D bug;
	inline static Texture2D cracker;
	inline static Texture2D player;
	inline static Texture2D vs;
	inline static Texture2D drone;
	inline static Texture2D medkit;
	inline static Texture2D grenade;
	inline static Texture2D bullet;
};

Texture2D loadTexture(u8 *image_data, u64 image_size, RenderContext::Format format = RenderContext::Format::u32_unorm, int force_components = 0);
Texture2D loadTextureFromFile(const char *filename, const std::string& extension = "png", RenderContext::Format format = RenderContext::Format::u32_unorm, int force_components = 0);
Shader loadShaderFromFile(std::string shader_name);
SpriteAnimationGroup loadSpriteAnimationGroupFromFile(const std::string& anim_path, const std::string& group_name); // path of folder
SpriteAnimation loadSpriteAnimationFromFile(const std::string& anim_path);