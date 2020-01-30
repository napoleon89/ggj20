#pragma once

#include <core/math.h>
#include <core/render_context.h>
#include <engine/physics.h>

struct Platform;
struct Input;
struct PlatformWindow;

REFLECT_STRUCT
struct Camera {
	REFLECT_FIELD Vec2 position;
	Mat4 projection;
	Vec2 size;

	void onResize(u32 window_width, u32 window_height);
	Mat4 getView(bool include_translation = true) const;
	Mat4 getViewProjection(bool include_translation = true) const;
	Vec2 transformWindowPosition(const Vec2 window_position);

	AABB getBounds() const;
};
