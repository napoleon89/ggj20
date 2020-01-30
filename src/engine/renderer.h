#pragma once

#include <core/std.h>
#include <core/math.h>
#include <core/collections/dynamic_array.h>
#include <core/render_context.h>
#include <engine/gui_renderer.h>
#include <engine/physics.h>


struct Assets;
struct SpriteAnimationGroup;

struct RenderSprite {
	Texture2D* sheet;
	Vec2 uv_offset;
	Vec2 uv_scale = Vec2(1.0f);
	Vec2 position;
	Vec2 size;
	bool flip_x;
	Color color = Colors::white;

	AABB getAABB() const;
};

struct RenderFrame {
	Mat4 camera_projection;
	Mat4 camera_view;
	Vec2 camera_position;
	AABB camera_bounds;
	Vec2 world_mouse_pos;
	GUIFrame gui_frame;
	DynamicArray<RenderSprite> background_sprites;
	DynamicArray<RenderSprite> sprites;
	f32 frame_time;
};

struct Renderer {
	RenderFrame frames[2];
	u32 frame_index = 0;

	void initResources();
	RenderFrame *getNewRenderFrame();
	void onResize(u32 width, u32 height);
	void processRenderFrame(RenderFrame *frame);
};