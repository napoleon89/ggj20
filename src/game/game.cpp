#include <game/game.h>
#include <core/std.h>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <vector>
#include <time.h>

#include <core/math.h>

#include <engine/input.h>
#include <engine/allocators.h>
#include <engine/audio.h>
#include <engine/debug_renderer.h>
#include <engine/timer.h>
#include <engine/profiler.h>
#include <engine/renderer.h>
#include <engine/physics.h>
#include <engine/gui/elements/lists.h>

#include <imgui/imgui.h>
#include <entt/entity/helper.hpp>

#include <game/assets.h>
#include <game/FastNoise.h>

extern StackAllocator g_frame_stack;

bool Globals::show_colliders = false;
bool Globals::anim_finished = false;
bool Globals::wireframe = false;


struct Transform {
	Vec2 position;
	Vec2 scale;
	f32 rotation;
};

Vec2 GameState::calcRandomPosition() {
	f32 x = Math::randRange(-10.0f, 10.0f);
	f32 y = Math::randRange(-10.0f, 10.0f);
	return Vec2(x, y);
}

Bullet* GameState::allocBullet() {
	for(u32 i = 0; i < bullet_pool_size; i++) {
		BulletState& state = bullet_pool_states[i];
		if(state == BulletState::Inactive) {
			state = BulletState::Active;
			return &bullet_pool[i];
		}
	}
	Assert(1 == 0); // Not enough space in pool
	return nullptr;
}

void GameState::freeBullet(Bullet* bullet) {
	for(u32 i = 0; i < bullet_pool_size; i++) {
		Bullet* lookup = &bullet_pool[i];
		if(lookup == bullet) {
			freeBullet(i);
			return;
		}
	}
}

void GameState::freeBullet(u32 index) {
	Bullet& lookup = bullet_pool[index];
	lookup = Bullet();
	bullet_pool_states[index] = BulletState::Inactive;
}

static Vec2 getInputAxis() {
	Vec2 result = Vec2();
	result.x += Input::isKeyDown(Key::D);
	result.x -= Input::isKeyDown(Key::A);
	result.y += Input::isKeyDown(Key::W);
	result.y -= Input::isKeyDown(Key::S);
	
	result.x += Input::isKeyDown(Key::Right);
	result.x -= Input::isKeyDown(Key::Left);
	result.y += Input::isKeyDown(Key::Up);
	result.y -= Input::isKeyDown(Key::Down);

	result += Input::getAnalogStick(AnalogStick::Left);
	
	result = Vec2::normalize(result);
	return result;
}

static bool getAttackButton() {
	bool result = false;
	result |= Input::isKeyDownOnce(Key::E);
	result |= Input::isControllerButtonDownOnce(ControllerButton::X);
	return result;
}

void GameState::init(PlatformWindow *window, AudioEngine *audio) {
	Platform::window_resize_event.addMemberCallback<GameState, &GameState::onResize>(this);
	timer = 0.0f;
	u32 window_width, window_height;
	Platform::getWindowSize(window, window_width, window_height);
	camera.onResize(window_width, window_height);
	bugs.add({Vec2(5.0f, 5.0f)});
	bugs.add({Vec2(-5.0f, -5.0f)});
	bugs.add({Vec2(5.0f, -5.0f)});
	bugs.add({Vec2(5.0f, 5.0f)});
	cracks.add({Vec2(-10.0f, -10.0f)});
	cracks.add({Vec2(-10.0f, 10.0f)});
	cracks.add({Vec2(10.0f, -10.0f)});
	cracks.add({Vec2(10.0f, 10.0f)});
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

	Vec2 fire_vector = Input::getAnalogStick(AnalogStick::Right);
	if(Vec2::lengthSquared(fire_vector) > 0) {
		Vec2 safe_vector = Vec2::normalize(fire_vector);
		Vec2 start = player.position + Vec2(0.0f, 1.0f);
		Vec2 end = start + safe_vector * 3.0f;
		DebugRenderQueue::addLine(start, end, Colors::green, 0.6f);

		if(Input::isControllerButtonDownOnce(ControllerButton::R1)) {
			Bullet* bullet = allocBullet();
			bullet->direction = safe_vector;
			bullet->position = end;
		}
	}

	player.velocity += getInputAxis() * 10.0f;
	player.velocity *= 0.5f;
	player.position += player.velocity * delta;

	DebugRenderQueue::addAABB(player.position - Vec2(0.5f, 0.0f), player.position + Vec2(0.5f, 2.0f), Colors::red);
	DebugRenderQueue::addCircle(player.position, 1.0f, Colors::red);


	for(u32 i = 0; i < bullet_pool_size; i++) {
		BulletState state = bullet_pool_states[i];
		if(state == BulletState::Active) {
			Bullet& bullet = bullet_pool[i];

			bullet.position += bullet.direction * 50.0f * delta;
			bullet.lifetime += delta;
			if(bullet.lifetime > 2.0f) {
				freeBullet(i);
			} else {
				DebugRenderQueue::addCircle(bullet.position, Bullet::radius, Colors::fuchsia, 0.1f);
			}
		}
	}

	for(u32 i = 0; i < bugs.count; i++) {
		Bug& bug = bugs.getRef(i);
		const f32 bug_radius = 1.0f;
		Color color = Colors::yellow;
		for(u32 bullet_index = 0; bullet_index < bullet_pool_size; bullet_index++) {
			BulletState state = bullet_pool_states[bullet_index];
			if(state == BulletState::Active) {
				Bullet& bullet = bullet_pool[bullet_index];
				if(Circle::intersects({bug.position, bug_radius}, {bullet.position, Bullet::radius})) {
					bug.velocity += bullet.direction * 20.0f;
					freeBullet(bullet_index);
					color = Colors::red;
					
				}
			}
		}

		f32 distance_to_target = Vec2::lengthSquared(bug.position - bug.target);
		if(distance_to_target <= 1.0f || bug.target == Vec2()) {
			bug.target = calcRandomPosition();
		}

		bug.velocity += (bug.target - bug.position) * 0.25f;

		bug.velocity *= 0.5f;
		bug.position += bug.velocity * delta;

		DebugRenderQueue::addCircle(bug.position, bug_radius, color);

	}

	for(u32 i = 0; i < cracks.count; i++) {
		Crack& crack = cracks.getRef(i);
		crack.timer -= delta;

		if(crack.timer <= 0.0f) {
			Vec2 start_point = crack.origin;
			if(crack.points.count > 0) {
				start_point = crack.points.getCopy(crack.points.count-1);
			}

			Vec2 dir = -Vec2::normalize(start_point);
			f32 fov = Math::toRadians(45.0f);
			f32 rand_fov = Math::randRange(-fov, fov);
			dir = Vec2::rotate(dir, rand_fov);
			dir = Vec2::normalize(dir);
			
			crack.points.add(start_point + dir * Math::randFloat() * 3.0f);

			crack.timer = Math::randRange(0.0f, 2.0f);
		}

		DebugRenderQueue::addCircle(crack.origin, 0.5f, Colors::red);
		Vec2 previous_point = crack.origin;
		for(u32 point_index = 0; point_index < crack.points.count; point_index++) {
			const Vec2& point = crack.points.getRefConst(point_index);
			DebugRenderQueue::addLine(previous_point, point, Colors::red, 1.0f);
			previous_point = point;
		}
	}

	camera.position += (player.position - camera.position) * 0.99999f * delta;
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
