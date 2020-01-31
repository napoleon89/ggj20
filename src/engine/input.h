#pragma once

#include <core/std.h>
#include <core/platform.h>
#include <core/math.h>

struct ButtonState {
	bool is_down;
	bool was_down;
};

REFLECT_STRUCT
struct Thing {

	REFLECT_FIELD
	Vec2 some_random_thing;
};

REFLECT_STRUCT
struct Settings {

	REFLECT_FIELD
	inline static Vec2 window_size = Vec2(1600.0f, 900.0f);

	REFLECT_FIELD
	inline static Thing ok;

	REFLECT_FIELD
	inline static bool fullscreen = 0;

	REFLECT_FIELD
	inline static std::string window_title;
};

struct MouseState {
	union {
		ButtonState buttons[3];

		struct {
			ButtonState left;
			ButtonState middle;
			ButtonState right;
		};
	};

	s32 x, y;
};

enum class AnalogStick {
	Left,
	Right
};

enum class Trigger {
	Left,
	Right,
};

struct ControllerState {
	ButtonState buttons[(u32)ControllerButton::Count];
};

struct Input {
private:
	inline static ButtonState buttons[2][(u32)Key::KeyCount];
	inline static ButtonState (*old_buttons)[(u32)Key::KeyCount];
	inline static ButtonState (*new_buttons)[(u32)Key::KeyCount];
	inline static MouseState mouse_states[2];
	inline static MouseState* old_mouse = 0;
	inline static MouseState* new_mouse = 0;
	inline static ControllerHandle controller = nullptr;
	inline static ControllerState controller_states[2];
	inline static ControllerState* old_controller = nullptr;
	inline static ControllerState* new_controller = nullptr;

public:

	static void init();

	static bool isKeyDown(Key key);
	static bool isKeyDownOnce(Key key);

	// relative to window
	static Vec2i getMousePosition();
	static Vec2i getMouseDelta();
	static bool isMouseButtonDown(MouseButton button);
	static bool isMouseButtonDownOnce(MouseButton button);
	static bool wasMouseButtonDownOnce(MouseButton button);

	// relative to window
	static void setMousePosition(s32 x, s32 y);
	static void processKeys();
	static void endFrame();
	static f32 getScrollAxis(Platform* platform);

	static void onControllerAdded(const u32 index);
	static Vec2 getAnalogStick(const AnalogStick stick);
	static f32 getTrigger(const Trigger trigger);
	static bool isControllerButtonDown(const ControllerButton button);
	static bool isControllerButtonDownOnce(const ControllerButton button);
};
