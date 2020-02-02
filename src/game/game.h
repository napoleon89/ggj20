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
struct FlexibleElement;

struct Globals {
	static bool show_colliders;
	static bool anim_finished;
	static bool wireframe;
};

struct PlayerState {
	Vec2 position;
	Vec2 velocity;
	u32 grenades = 0;
	static constexpr f32 radius = 1.0f;
};

enum class BulletState {
	Inactive,
	Active,
};

struct Bullet {
	Vec2 position;
	Vec2 direction;
	f32 lifetime;
	inline static f32 radius = 0.5f;
};

enum class BugMode {
	Attacking,
	Retreating,
};

struct Bug {
	Vec2 velocity;
	Vec2 target;
	BugMode mode = BugMode::Attacking;
};

struct Crack {
	DynamicArray<Vec2> points;
	f32 timer = 0.0f;
	f32 max_timer= 0.0f;
};

struct Grenade {
	f32 radius;
	f32 radius_velocity;
	f32 max_radius = 8.0f;
};

struct Position {
	Vec2 position;
	Vec2 padding;
};

enum class PickupType {
	Grenade,
	HealthPack,
};

struct Pickup {
	PickupType type;
};

struct EnemyHealth {
	f32 collision_radius;
	f32 health = 100.0f;
	f32 max_health = 100.0f;
	Color color = Colors::red;
	bool damaged = false;
};

struct Particle {
	Color color;
	Vec2 velocity;
	Vec2 dir;
	f32 speed;
	f32 lifetime;
};

enum class StateType {
	PreGame,
	Game,
	EndGame,
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
	f32 bullet_spawn_timer = 0.0f;
	f32 crack_spawn_timer = 0.0f;
	f32 bug_spawn_timer = 0.0f;
	f32 difficulty = 1.0f;
	DynamicArray<entt::entity> destroy_list;
	bool draw_drone;
	Vec2 drone_pos;

	StateType type = StateType::PreGame;
	
	static constexpr f32 haven_max_health = 100.0f;
	f32 haven_health = haven_max_health;

	static constexpr f32 arena_radius = 20.0f;
	inline static Circle haven = {Vec2(), 3.0f};

	FlexibleElement* haven_health_bar;
	FlexibleElement* haven_health_bar_back;
	Element* grenades_element;
	Element* game_over_element;
	Element* intro_element;

	Bullet* allocBullet();
	void freeBullet(Bullet* bullet);
	void freeBullet(u32 index);
	Vec2 calcRandomPosition();
	Vec2 calcRandomCrackPoint();

	void createCrack();
	void createBug();

	void init(PlatformWindow *window, AudioEngine *audio);
	void updateGame(f32 delta, PlatformWindow *window);
	void update(f32 delta, PlatformWindow *window);
	void render(RenderFrame *render_frame, PlatformWindow *window, f32 delta);
	void onResize(u32 new_width, u32 new_height);

};