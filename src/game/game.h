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

struct GameState {
	f32 timer;
	Camera camera;
	PhysicsWorld physics_world;
	ElementLayoutTree gui_tree;
	entt::registry registry;

	void init(PlatformWindow *window, AudioEngine *audio);
	void update(f32 delta, PlatformWindow *window);
	void render(RenderFrame *render_frame, PlatformWindow *window, f32 delta);
	void onResize(u32 new_width, u32 new_height);

};