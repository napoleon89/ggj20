#pragma once

#include <core/math.h>
#include <core/render_context.h>
#include <engine/physics.h>

struct Platform;
struct Input;
struct PlatformWindow;

REFLECT_STRUCT
struct Camera {
private:
	Vec2 position;
	Vec2 shake_offset;
public:	
	Mat4 projection;
	Vec2 size;
	

	void onResize(u32 window_width, u32 window_height);
	Mat4 getView(bool include_translation = true) const;
	Mat4 getViewProjection(bool include_translation = true) const;
	Vec2 transformWindowPosition(const Vec2 window_position);
	void shake(Vec2 amount);
	Vec2 getPosition() const;
	void setPosition(Vec2 position);
	void update(f32 delta);

	AABB getBounds() const;
};
