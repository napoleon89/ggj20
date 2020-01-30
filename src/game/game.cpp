#include <game/game.h>
#include <core/std.h>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <core/math.h>
//#include <imgui/ImGuizmo.h>
#include <engine/input.h>
#include <float.h>
#include <string.h>
#include <vector>
#include <engine/allocators.h>
#include <time.h>
#include <engine/audio.h>
#include <game/assets.h>
#include <engine/debug_renderer.h>
#include <game/FastNoise.h>
#include <time.h>
#include <imgui/imgui.h>
#include <engine/timer.h>
#include <engine/profiler.h>
#include <engine/renderer.h>
#include <engine/physics.h>
#include <engine/gui/elements/lists.h>
#include <entt/entity/helper.hpp>

extern StackAllocator g_frame_stack;

bool Globals::show_colliders = false;
bool Globals::anim_finished = false;
bool Globals::wireframe = false;





void GameState::init(PlatformWindow *window, AudioEngine *audio) {
	Platform::window_resize_event.addMemberCallback<GameState, &GameState::onResize>(this);
	timer = 0.0f;
	u32 window_width, window_height;
	Platform::getWindowSize(window, window_width, window_height);
	camera.onResize(window_width, window_height);
}

void GameState::update(f32 delta, PlatformWindow *window) {
	ProfileFunc;
	timer += delta;
	
	Vec2 mouse_pos = camera.transformWindowPosition(v2iToV2(Input::getMousePosition()));
	
	static bool show_window = false;
	if(Input::isKeyDownOnce(Key::F3)) {
		show_window = !show_window;
	}

	if(show_window) {
		ImGui::ShowDemoWindow(&show_window);
	}
}

static RenderSprite genSpriteFromAnimMachine(const AnimationMachine& machine, const Vec2 frame_scale) {
	RenderSprite result = {&machine.current_animation->sheet};
	
	Vec2 uv_offset = machine.current_animation->getFrameUVOffset(machine.frame_index);
	Vec2 uv_scale = machine.current_animation->getFrameUVScale();
	
	result.size = machine.current_animation->frame_size * frame_scale;
	result.uv_offset = uv_offset;
	result.uv_scale = uv_scale;
	return result;
}

void GameState::render(RenderFrame *render_frame, PlatformWindow *window, f32 delta) {
	ProfileFunc;

	render_frame->camera_projection = camera.projection;
	render_frame->camera_view = camera.getView();
	render_frame->camera_bounds = camera.getBounds();
	render_frame->camera_position = camera.position;
	render_frame->frame_time = delta;
	render_frame->world_mouse_pos = camera.transformWindowPosition(v2iToV2(Input::getMousePosition()));

	{
		ProfileScope("GUiProcess");
		gui_tree.processLayout();
		gui_tree.processPaint(&render_frame->gui_frame);
	}
}

void GameState::onResize(u32 new_width, u32 new_height) {
	camera.onResize(new_width, new_height);
}
