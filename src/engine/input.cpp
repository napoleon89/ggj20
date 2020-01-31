#include <engine/input.h>

#include <core/collections/dynamic_array.h>
#include <core/logger.h>
#include <engine/profiler.h>
#include <imgui/imgui.h>

void Input::init() {
	old_mouse = &mouse_states[0];
	new_mouse = &mouse_states[1];
	old_buttons = &buttons[0];
	new_buttons = &buttons[1];

	old_controller = &controller_states[0];
	new_controller = &controller_states[1];

	Platform::controller_added_event.addCallback(&Input::onControllerAdded);
}


bool Input::isKeyDown(Key key) {
	return (*new_buttons)[(int)key].is_down;
}

bool Input::isKeyDownOnce(Key key) {
	return ((*new_buttons)[(int)key].is_down && !(*new_buttons)[(int)key].was_down);
}

// relative to window
Vec2i Input::getMousePosition() {
	return Vec2i(new_mouse->x, new_mouse->y);
}

Vec2i Input::getMouseDelta() {
	return getMousePosition() - Vec2i(old_mouse->x, old_mouse->y);
}

bool Input::isMouseButtonDown(MouseButton button) {
	return new_mouse->buttons[(s32)button].is_down;
}

bool Input::isMouseButtonDownOnce(MouseButton button) {
	return new_mouse->buttons[(s32)button].is_down && !new_mouse->buttons[(s32)button].was_down;
}

bool Input::wasMouseButtonDownOnce(MouseButton button) {
	return new_mouse->buttons[(s32)button].was_down && !new_mouse->buttons[(s32)button].is_down;
}


// relative to window
void Input::setMousePosition(s32 x, s32 y) {
	Platform::setMousePosition(x, y);
}

void Input::processKeys() {
	ProfileFunc;

	for(int i = 0; i < (int)Key::KeyCount; i++) {
		ButtonState* old_input = old_buttons[0] + i;
		ButtonState* new_input = new_buttons[0] + i;

		new_input->is_down = Platform::getKeyDown((Key)i);
		new_input->was_down = old_input->is_down;
	}

	Platform::getMousePosition(new_mouse->x, new_mouse->y);

	if(!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive()) {
		for(u32 i = 0; i < 3; i++) {
			new_mouse->buttons[i].is_down = Platform::getMouseDown((MouseButton)i);
			new_mouse->buttons[i].was_down = old_mouse->buttons[i].is_down;
		}
	}

	for(u32 i = 0; i < (u32)ControllerButton::Count; i++) {
		new_controller->buttons[i].is_down = Platform::getControllerButton(controller, (ControllerButton)i);
		new_controller->buttons[i].was_down = old_controller->buttons[i].is_down;
	}
}

void Input::endFrame() {
	Swap(old_buttons, new_buttons);
	Swap(old_mouse, new_mouse);
	Swap(old_controller, new_controller);
}

f32 getScrollAxis() {
	return Platform::getMouseWheel();
}

void Input::onControllerAdded(const u32 index) {
	Log::info("Controller %d added", index);
	if(index == 0) {
		controller = Platform::openController(index);	
	}
}

Vec2 Input::getAnalogStick(const AnalogStick stick) {
	Vec2 result = Vec2();
	ControllerAxis x_axis, y_axis;
	switch(stick) {
		case AnalogStick::Left: { x_axis = ControllerAxis::LeftX; y_axis = ControllerAxis::LeftY; } break;
		case AnalogStick::Right: { x_axis = ControllerAxis::RightX; y_axis = ControllerAxis::RightY; } break;
	}
	
	f32 x = Platform::getControllerAxis(controller, x_axis);
	f32 y = -Platform::getControllerAxis(controller, y_axis);
	const f32 deadzone = 0.1f;
	if(!Math::inRange(x, -deadzone, deadzone)) result.x = x;
	if(!Math::inRange(y, -deadzone, deadzone)) result.y = y;
	
	// result = Vec2::normalize(result);
	return result;
}

f32 Input::getTrigger(const Trigger trigger) {
	ControllerAxis axis;
	switch(trigger) {
		case Trigger::Left: axis = ControllerAxis::LeftTrigger; break;
		case Trigger::Right: axis = ControllerAxis::RightTrigger; break;
	}

	f32 value = Platform::getControllerAxis(controller, axis);
	const f32 deadzone = 0.3f;
	// if(value >= deadzone) {
		return value;
	// }
	// return 0.0f;
}

bool Input::isControllerButtonDown(const ControllerButton button) {
	return new_controller->buttons[(u32)button].is_down;
}

bool Input::isControllerButtonDownOnce(const ControllerButton button) {
	const ButtonState& state = new_controller->buttons[(u32)button];
	return state.is_down && !state.was_down;
	
}
