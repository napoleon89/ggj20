#include <game/camera.h>
#include <engine/input.h>
#include <core/platform.h>


void Camera::onResize(u32 window_width, u32 window_height) {
	
	f32 aspect_ratio = (f32)window_width / (f32)window_height;
	size = Vec2(50.0f, 50.0f / aspect_ratio);

	projection = Mat4::ortho(size.x, size.y, -1000.0f, 1000.0f);
}

Mat4 Camera::getView(bool include_translation) const {
	return Mat4::translate(Vec3(-position, 0.0f));
}

Mat4 Camera::getViewProjection(bool include_translation) const {
	return projection * getView(include_translation);
}

Vec2 Camera::transformWindowPosition(const Vec2 window_position) {
	Vec2 result = window_position;
	u32 window_width, window_height;
	Platform::getWindowSize(Platform::getCurrentWindow(), window_width, window_height);
	Vec2 window_size = Vec2(window_width, window_height);
	Vec2 mul = window_size / (size * 2.0f);
	
	result /= window_size;
	result.y = 1.0f - result.y;
	result -= Vec2(0.5f, 0.5f);
	result *= size;
	result += position;
	
	return result;
}

AABB Camera::getBounds() const {
	Vec2 half_size = size * 0.5f;
	return {position - half_size, position + half_size};
}
