#pragma once

#include <vector>
#include <core/math.h>
#include <engine/static_mesh.h>
#include <engine/animation2d.h>
//#include <game/ImGuizmo.h>
#include <game/camera.h>


struct Input;
struct RenderContext;
struct Assets;
struct GameState;

struct GameState;

struct EditorState {
	bool enabled;
	bool was_enabled;
	Camera start_camera;
	GameState *game;
	f32 timer;
	bool show_profiler;
	//ImGuizmo::OPERATION operation;
	AnimationMachine anim_machine;
	
	void init();
	void onShow(GameState *g);
	void onHide();
	void update(f32 delta, PlatformWindow *window);
	void render(PlatformWindow *window, f32 delta);
};