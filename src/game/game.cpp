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
	f32 x = Math::randRange(-15.0f, 15.0f);
	f32 y = Math::randRange(-15.0f, 15.0f);
	return Vec2(x, y);
}

Vec2 GameState::calcRandomCrackPoint() {
	while(true) { // ah fuck
		f32 angle = Math::randRange(0.0f, Math::Pi32 * 2.0f);
		Vec2 position = Vec2(Math::cos(angle) * arena_radius, Math::sin(angle) * arena_radius);
		bool too_close = false;
		registry.view<Position, Crack>().each([&](Position& pos, Crack& crack) {
			f32 distance_squared = Vec2::lengthSquared(pos.position - position);
			if(distance_squared <= 2.0f * 2.0f) {
				too_close = true;
				return;
			} 
		});

		if(!too_close) { 
			return position;
		}
	}
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

void GameState::createCrack() {
	Vec2 pos = calcRandomCrackPoint();
	entt::entity ent = registry.create();
	EnemyHealth& eh = registry.assign<EnemyHealth>(ent, 1.0f);
	f32 d = 1.0f / 255.0f;
	eh.color = Color(1.0f, d *  230.0f, d * 213.0f);
	registry.assign<Crack>(ent);
	registry.assign<Position>(ent, pos);
}

void GameState::createBug() {
	Vec2 pos = calcRandomPosition();
	entt::entity ent = registry.create();
	registry.assign<Position>(ent, pos);
	f32 d = 1.0f / 255.0f;
	registry.assign<EnemyHealth>(ent, 1.0f, 50.0f, 50.0f, Color(d * 170.0f, d * 255.0f, d * 170.0f));
	registry.assign<Bug>(ent);
}


void GameState::init(PlatformWindow *window, AudioEngine *audio) {
	Platform::window_resize_event.addMemberCallback<GameState, &GameState::onResize>(this);
	timer = 0.0f;
	u32 window_width, window_height;
	Platform::getWindowSize(window, window_width, window_height);
	camera.onResize(window_width, window_height);

	gui_tree.root.layout_data.padding_pixels = 10.0f;

	FlexibleListElement* haven_health_wrapper = gui_tree.root.addChild<FlexibleListElement>();
	haven_health_wrapper->layout_data.height_type = SizeType::Absolute;
	haven_health_wrapper->layout_data.height = 32.0f;
	// haven_health_wrapper->color = Colors::navy;
	haven_health_wrapper->orientation = FlexibleListElement::Orientation::Horizontal;
	{
		Element* haven_health_title = haven_health_wrapper->addChild<Element>();
		haven_health_title->text = "Stability";
		haven_health_title->font_size = 32.0f;
		haven_health_title->layout_data.width_type = SizeType::Absolute;
		haven_health_title->layout_data.width = 150.0f;
		haven_health_title->text_align_hor = HorizontalTextAlign::Center;
		haven_health_title->text_align_ver = VerticalTextAlign::Center;

		haven_health_bar = haven_health_wrapper->addChild<FlexibleElement>();
		haven_health_bar->color = Color(0.0f, 0.5f, 0.0f, 0.75f);
		haven_health_bar->flex_factor = 1.0f;

		haven_health_bar_back = haven_health_wrapper->addChild<FlexibleElement>();
		haven_health_bar_back->color = Color(0.5f, 0.0f, 0.0f, 0.75f);
		haven_health_bar_back->flex_factor = 0.0f;

	}

	FlexibleListElement* grenades_wrapper = gui_tree.root.addChild<FlexibleListElement>();
	grenades_wrapper->orientation = FlexibleListElement::Orientation::Vertical;
	grenades_wrapper->layout_data.width_type = SizeType::Absolute;
	grenades_wrapper->layout_data.width = 200.0f;

	{
		grenades_wrapper->addChild<FlexibleElement>();
		grenades_element = grenades_wrapper->addChild<Element>();
		grenades_element->layout_data.height_type = SizeType::Absolute;
		grenades_element->layout_data.height = 60.0f;
		grenades_element->text = "Bombs: 0";
		grenades_element->font_size = 32.0f;
		grenades_element->text_align_hor = HorizontalTextAlign::Center;
		grenades_element->text_align_ver = VerticalTextAlign::Center;
	}

	game_over_element = gui_tree.root.addChild<Element>();
	game_over_element->layout_data.pivot_x = 0.5f;
	game_over_element->layout_data.pivot_y = 0.5f;
	game_over_element->layout_data.width = 0.75f;
	game_over_element->layout_data.height = 0.6f;
	game_over_element->layout_data.x = 0.5f;
	game_over_element->layout_data.y = 0.5f;
	game_over_element->color = Color(1.0f, 0.0f, 0.0f, 0.5f);
	game_over_element->text_align_hor = HorizontalTextAlign::Center;
	game_over_element->text_align_ver = VerticalTextAlign::Top;
	game_over_element->text_color = Colors::white;
	game_over_element->font_size = 128.0f;
	game_over_element->visible = false;
	game_over_element->text = "You failed to stabilize VS2020... It's okay, we're going to ship it anyway!";

	intro_element = gui_tree.root.addChild<Element>();
	intro_element->layout_data.pivot_x = 0.5f;
	intro_element->layout_data.pivot_y = 0.5f;
	intro_element->layout_data.width = 0.75f;
	intro_element->layout_data.height = 0.25f;
	intro_element->layout_data.x = 0.5f;
	intro_element->layout_data.y = 0.5f;
	intro_element->border_radius = 32.0f;
	f32 d = 1.0f / 255.0f;
	intro_element->color = Color(d * 104.0f, d * 32.0f, d * 121.0f);
	intro_element->text_align_hor = HorizontalTextAlign::Center;
	intro_element->text_align_ver = VerticalTextAlign::Center;
	intro_element->text_color = Colors::white;
	intro_element->font_size = 64.0f;
	intro_element->text = "Press X to start the game";
}
void GameState::update(f32 delta, PlatformWindow *window) {
	switch(type) {
		case StateType::PreGame: {
			

			if(Input::isControllerButtonDownOnce(ControllerButton::A)) {
				type = StateType::Game;
				intro_element->visible = false;
			}
		}; break;

		case StateType::Game: {
			
			updateGame(delta, window);
			if(haven_health <= 0.0f) {
				type = StateType::EndGame;
				game_over_element->visible = true;
			}
		} break;

		case StateType::EndGame: {
			
		}; break;
	}
}


void GameState::updateGame(f32 delta, PlatformWindow *window) {
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

	bullet_spawn_timer += delta;
	draw_drone = false;
	Vec2 fire_vector = Input::getAnalogStick(AnalogStick::Right);
	if(Vec2::lengthSquared(fire_vector) > 0) {
		Vec2 safe_vector = Vec2::normalize(fire_vector);
		Vec2 start = player.position + Vec2(0.0f, 1.0f);
		Vec2 end = start + safe_vector * 3.0f;
		// DebugRenderQueue::addLine(start, end, Colors::green, 0.6f);
		draw_drone = true;
		drone_pos = end;

		if(Input::getTrigger(Trigger::Right) > 0.0f && bullet_spawn_timer >= 0.1f) {
			Bullet* bullet = allocBullet();
			Vec2 dir = safe_vector;
			f32 rand_ang = Math::toRadians(Math::randRange(-5.0f, 5.0f));
			dir = Vec2::rotate(dir, rand_ang);
			bullet->direction = dir;
			bullet->position = end;
			bullet_spawn_timer = 0.0f;
			player.velocity += -dir * 20.0f;
			// camera.shake(-safe_vector * 0.25f);
		}
	} 

	if(Input::isControllerButtonDownOnce(ControllerButton::R1) && player.grenades > 0) {
		entt::entity entity = registry.create();
		registry.assign<Position>(entity, player.position);
		registry.assign<Grenade>(entity);
		player.grenades -= 1;
	}

	grenades_element->text = "Bombs: " + std::to_string(player.grenades);

	player.velocity += getInputAxis() * 10.0f;
	player.velocity *= 0.5f;
	player.position += player.velocity * delta;

	// DebugRenderQueue::addAABB(player.position - Vec2(0.5f, 0.0f), player.position + Vec2(0.5f, 2.0f), Colors::red);
	// DebugRenderQueue::addCircle(player.position, player.radius, Colors::red);


	for(u32 i = 0; i < bullet_pool_size; i++) {
		BulletState state = bullet_pool_states[i];
		if(state == BulletState::Active) {
			Bullet& bullet = bullet_pool[i];

			bullet.position += bullet.direction * 50.0f * delta;
			bullet.lifetime += delta;
			if(bullet.lifetime > 2.0f) {
				freeBullet(i);
			} else {
				// DebugRenderQueue::addCircle(bullet.position, Bullet::radius, Colors::fuchsia, 0.1f);
			}
		}
	}

	registry.view<Grenade, Position>().each([&](const entt::entity& entity, Grenade& grenade, Position& position) {
		if(grenade.radius >= grenade.max_radius) {
			registry.view<EnemyHealth, Position>().each([&](const entt::entity& enemy_ent, EnemyHealth& eh, Position& enemy_pos) {
				if(Circle::intersects({position.position, grenade.radius}, {enemy_pos.position, eh.collision_radius})) {
					eh.health -= 40.0f;
					if(registry.has<Bug>(enemy_ent)) {
						Bug& bug = registry.get<Bug>(enemy_ent);
						Vec2 expl_dir = Vec2::normalize(enemy_pos.position - position.position);
						bug.velocity += expl_dir * 80.0f;
					}
				}
			});
			destroy_list.add(entity);
			
		} else {
			grenade.radius_velocity += 7.25f;
			grenade.radius += grenade.radius_velocity * delta;
			// DebugRenderQueue::addCircle(position.position, grenade.radius, Colors::purple);
		}
	});

	auto spawn_blood = [&](const Vec2 position, const Color& color, f32 life_min = 0.5f, f32 life_max = 1.0f, f32 spawn_min = 32.0f, f32 spawn_max = 64.0f) {
		for(u32 i = 0; i < (u32)(Math::randRange(spawn_min, spawn_max)); i++) {
			entt::entity particle = registry.create();
			f32 angle = Math::randFloat() * (Math::Pi32 * 2.0f);
			Vec2 dir = Vec2(Math::cos(angle), Math::sin(angle));
			registry.assign<Position>(particle, position);
			registry.assign<Particle>(
				particle, 
				color, 
				Vec2(), 
				dir, 
				Math::randRange(20.0f, 40.0f), 
				Math::randRange(life_min, life_max)
			);
		}
	};


	registry.view<Bug, Position, EnemyHealth>().each([&](Bug& bug, Position& pos, EnemyHealth& eh)  {

		f32 distance_to_target = Vec2::lengthSquared(pos.position - bug.target);
		if(distance_to_target <= 1.0f) {
			if(bug.mode == BugMode::Attacking) {
				bug.target = calcRandomPosition();
				bug.mode = BugMode::Retreating;
				haven_health -= 1.0f;
				f32 d = 1.0f / 255.0f;
				spawn_blood(pos.position, Color(d * 104.0f, d * 32.0f, d * 121.0f), 0.1f, 0.25f, 5, 10);
			} else {
				bug.mode = BugMode::Attacking;
				bug.target = Vec2();
			}
		}

		f32 speed = (bug.mode == BugMode::Attacking) ? 0.5f : 0.25f;
		bug.velocity += (bug.target - pos.position) * 0.25f;

		bug.velocity *= 0.9f;
		pos.position += bug.velocity * delta;

	});

	registry.view<Position, Crack>().each([&](Position& pos, Crack& crack) {
		const Vec2 position = pos.position;
		crack.timer -= delta;

		if(crack.timer <= 0.0f) {
			Vec2 start_point = position;
			if(crack.points.count > 0) {
				start_point = crack.points.getCopy(crack.points.count-1);
			}

			Vec2 dir = -Vec2::normalize(start_point);
			f32 fov = Math::toRadians(45.0f);
			f32 rand_fov = Math::randRange(-fov, fov);
			dir = Vec2::rotate(dir, rand_fov);
			dir = Vec2::normalize(dir);
			Vec2 new_point = start_point + dir * Math::randFloat() * 3.0f;
			crack.points.add(new_point);

			if(haven.pointInside(start_point)) {
				haven_health -= 5.0f;
			}

			crack.timer = Math::randRange(3.0f, 10.0f);
			crack.max_timer = crack.timer;
		}
	});

	registry.view<EnemyHealth, Position>().each([&](const entt::entity& entity, EnemyHealth& eh, Position& position) {
		if(eh.health <= 0.0f) {
			if(Math::randFloat() > 0.9f) {
				entt::entity entity = registry.create();
				registry.assign<Position>(entity, position.position);
				PickupType ptype = (Math::randFloat() > 0.5f) ? PickupType::HealthPack : PickupType::Grenade;
				registry.assign<Pickup>(entity, ptype);
			}
			
			spawn_blood(position.position, eh.color);

			destroy_list.add(entity);
		}
	});

	if(Input::isMouseButtonDownOnce(MouseButton::Left)) {
		spawn_blood(mouse_pos, Colors::red);
	}

	registry.view<Pickup, Position>().each([&](const entt::entity& entity, Pickup& pickup, Position& position) {
		f32 radius = 1.0f;
		if(Circle::intersects({position.position, radius}, {player.position, player.radius})) {
			switch(pickup.type) {
				case PickupType::Grenade: { player.grenades += 1; } break;
				case PickupType::HealthPack: { haven_health += 10.0f; haven_health = Math::min(haven_health, haven_max_health); 	} break;
			}
			destroy_list.add(entity); 
		}
		
	});


	registry.view<Position, EnemyHealth>().each([&](const entt::entity& entity, Position& position, EnemyHealth& health) {
		for(u32 bullet_index = 0; bullet_index < bullet_pool_size; bullet_index++) {
			BulletState state = bullet_pool_states[bullet_index];
			if(state == BulletState::Active) {
				Bullet& bullet = bullet_pool[bullet_index];
				if(Circle::intersects({position.position, health.collision_radius}, {bullet.position, Bullet::radius})) {
					health.health -= 5.0f;
					if(registry.has<Bug>(entity)) {
						Bug& bug = registry.get<Bug>(entity);
						bug.velocity += bullet.direction * 40.0f;
					}
					freeBullet(bullet_index);
					health.damaged = true;
				}
			}
		}

		// DebugRenderQueue::addCircle(position.position, health.collision_radius, Colors::red);
	});

	registry.view<Position, Particle>().each([&](const entt::entity& entity, Position& position, Particle& particle) {
		particle.lifetime -= delta;
		if(particle.lifetime <= 0.0f) {
			destroy_list.add(entity);
		} else {
			particle.velocity += particle.dir * particle.speed;
			particle.velocity *= 0.5f;
			position.position += particle.velocity * delta;

			DebugRenderQueue::addLine(
				position.position, 
				position.position + particle.dir, 
				particle.color,
				0.25f
			);
		}
	});

	camera.setPosition(camera.getPosition() + (player.position - camera.getPosition()) * 0.9999999f * delta);
	camera.update(delta);

	crack_spawn_timer -= delta;
	if(crack_spawn_timer <= 0.0f) {
		crack_spawn_timer = Math::randRange(4.0f, 7.0f) * difficulty;
		createCrack();
	}

	bug_spawn_timer -= delta;
	if(bug_spawn_timer <= 0.0f) {
		bug_spawn_timer = Math::randRange(4.0f, 8.0f) * difficulty;
		createBug();
	}

	difficulty -= 0.005f * delta;
	difficulty = Math::max(difficulty, 0.1f);

	// DebugRenderQueue::addCircle(Vec2(), arena_radius, Colors::red);
	// DebugRenderQueue::addCircle(haven.center, haven.radius, Colors::green);

	haven_health_bar->flex_factor = haven_health / haven_max_health;
	haven_health_bar_back->flex_factor = 1.0f - haven_health_bar->flex_factor;

	for(const entt::entity& entity : destroy_list) {
		if(registry.has(entity) && registry.valid(entity)) {
			registry.destroy(entity);
		}
	}

	destroy_list.clear();
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
	render_frame->camera_position = camera.getPosition();
	render_frame->frame_time = delta;
	render_frame->world_mouse_pos = camera.transformWindowPosition(v2iToV2(Input::getMousePosition()));
	render_frame->game = this;

	registry.view<Bug, Position, EnemyHealth>().each([&](Bug& bug, Position& pos, EnemyHealth& eh)  {
		RenderSprite sprite = {};
		sprite.sheet = &Assets::bug;
		sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
		sprite.position = pos.position - sprite.size * 0.5f;
		sprite.color = (eh.damaged) ? Colors::red : Colors::white;
		render_frame->sprites.add(sprite);
		eh.damaged = false;
	});

	registry.view<Crack, Position, EnemyHealth>().each([&](Crack& crack, Position& pos, EnemyHealth& eh)  {
		RenderSprite sprite = {};
		sprite.sheet = &Assets::cracker;
		sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
		sprite.position = pos.position - sprite.size * 0.5f;
		sprite.color = (eh.damaged) ? Colors::red : Colors::white;
		render_frame->sprites.add(sprite);
		eh.damaged = false;
	});

	{
		RenderSprite sprite = {};
		sprite.sheet = &Assets::player;
		sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
		sprite.position = player.position - Vec2(sprite.size.x * 0.5f, 0.0f);
		
		render_frame->sprites.add(sprite);
	}

	{
		RenderSprite sprite = {};
		sprite.sheet = &Assets::vs;
		sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
		sprite.position = -(sprite.size * Vec2(0.5f, 0.5f));
		
		render_frame->background_sprites.add(sprite);
	}

	{
		if(draw_drone) {
			RenderSprite sprite = {};
			sprite.sheet = &Assets::drone;
			sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
			sprite.position = drone_pos - (sprite.size * Vec2(0.5f, 0.5f));
			render_frame->sprites.add(sprite);

		}
	}

	for(u32 i = 0; i < bullet_pool_size; i++) {
		BulletState state = bullet_pool_states[i];
		if(state == BulletState::Active) {
			Bullet& bullet = bullet_pool[i];
			RenderSprite sprite = {};
			sprite.sheet = &Assets::bullet;
			sprite.rotation = Vec2::angle(Vec2(0.0f, 1.0f), bullet.direction) + Math::Pi32 * 0.5f;
			sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
			sprite.position = bullet.position - (sprite.size * Vec2(0.5f, 0.5f));
			render_frame->sprites.add(sprite);
		}
	}

	registry.view<Pickup, Position>().each([&](const entt::entity& entity, Pickup& pickup, Position& position) {
		RenderSprite sprite = {};
		

		switch(pickup.type) {
			case PickupType::Grenade: { sprite.sheet = &Assets::grenade; } break;
			case PickupType::HealthPack: { sprite.sheet = &Assets::medkit; } break;
		}
		
		sprite.size = Vec2(sprite.sheet->width, sprite.sheet->height) / Vec2(50.0f, 50.0f);
		sprite.position = position.position - (sprite.size * Vec2(0.5f, 0.5f));
		render_frame->sprites.add(sprite);
	});


	{
		ProfileScope("GUiProcess");
		gui_tree.processLayout();
		gui_tree.processPaint(&render_frame->gui_frame);
	}
}

void GameState::onResize(u32 new_width, u32 new_height) {
	camera.onResize(new_width, new_height);
}
