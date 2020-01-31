#pragma once

struct Input;
struct AudioEngine;
struct ImGuiContext;
struct RenderFrame;

#include <core/math.h>
#include <game/camera.h>
#include <engine/physics.h>
#include <engine/animation2d.h>
#include <engine/gui/gui.h>
#include <entt/entity/registry.hpp>

struct Input;
struct PlatformWindow;
struct Platform;
struct AudioEngine;
struct Assets;
struct RenderFrame;

struct Globals {
	static bool show_colliders;
	static bool anim_finished;
	static bool wireframe;
};

struct PlayerState {
	Vec2 position;
	Vec2 velocity;
};

enum class BulletState {
	Inactive,
	Active,
};

struct Bullet {
	Vec2 position;
	Vec2 direction;
	f32 lifetime;
	inline static f32 radius = 0.25f;
};

struct Bug {
	Vec2 position;
	Vec2 velocity;
	Vec2 target;
};

struct Crack {
	Vec2 origin;
	DynamicArray<Vec2> points;
	f32 timer = 0.0f;
};

struct GameState {
	f32 timer;
	Camera camera;
	PhysicsWorld physics_world;
	ElementLayoutTree gui_tree;
	entt::registry registry;
	PlayerState player;
	static constexpr u32 bullet_pool_size = 128;
	BulletState bullet_pool_states[bullet_pool_size];
	Bullet bullet_pool[bullet_pool_size];
	DynamicArray<Bug> bugs;
	DynamicArray<Crack> cracks;

	Bullet* allocBullet();
	void freeBullet(Bullet* bullet);
	void freeBullet(u32 index);
	Vec2 calcRandomPosition();

	void init(PlatformWindow *window, AudioEngine *audio);
	void update(f32 delta, PlatformWindow *window);
	void render(RenderFrame *render_frame, PlatformWindow *window, f32 delta);
	void onResize(u32 new_width, u32 new_height);

};