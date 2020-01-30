#pragma once

#include <core/math.h>

struct RenderContext;
struct Shader;

struct DebugRenderQueue {
	static void init(Shader *debug_shader);
	static void addLine(const Vec2& start, const Vec2& end, const Color& color = Colors::white, f32 thickness = 0.05f);
	static void addAABB(const Vec2& min, const Vec2& max, const Color& color = Colors::white, f32 thickness = 0.05f);
	static void addCircle(const Vec2& center, const f32 radius, const Color& color = Colors::white, f32 thickness = 0.05f);
	static void startframe();
	static void flushRender(Shader *debug_shader, const Mat4 &view_projection);
};