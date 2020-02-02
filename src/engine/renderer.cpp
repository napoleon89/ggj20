#include "renderer.h"
#include <imgui/imgui.h>

#include <core/platform.h>
#include <core/math.h>

#include <engine/imgui_impl.h>
#include <engine/profiler.h>
#include <engine/static_mesh.h>
#include <engine/debug_renderer.h>
#include <nanovg/nanovg.h>
#include <nanovg/nanovg_gl_utils.h>
#define NANOSVG_IMPLEMENTATION
#include <nanovg/nanosvg.h>

#include <game/assets.h>
#include <game/game.h>


struct PerSceneConstants {
	Mat4 projection;
};

struct TexturedVertex {
	Vec2 position;
	Vec2 uv;
};


struct PerObjectConstants {
	Mat4 model;
	Color color;
	Vec2 uv_offset;
	Vec2 uv_scale;
};

struct SpriteInstanceData {
	Vec2 position;
	Vec2 size;
	Vec2 uv_offset;
	Vec2 uv_scale;
	Vec4 color;
};

global_variable VertexArray g_sprite_layout;
global_variable VertexArray g_instance_sprite_layout;


global_variable ShaderConstant g_per_scene_constants;
global_variable ShaderConstant g_per_object_constants;

global_variable Sampler g_texture_sampler;
global_variable Sampler g_wrap_texture_sampler;

AABB RenderSprite::getAABB() const {
	return {position, position + size};
}

void Renderer::initResources() {
	Platform::window_resize_event.addMemberCallback<Renderer, &Renderer::onResize>(this);
	DebugRenderQueue::init(&Assets::shaders.editor);


	g_sprite_layout = RenderContext::createVertexArray({
		{"POSITION", RenderContext::Format::Vec2},
		{"TEXCOORD", RenderContext::Format::Vec2}
	});

	g_per_scene_constants = RenderContext::createShaderConstant(sizeof(PerSceneConstants));
	g_per_object_constants = RenderContext::createShaderConstant(sizeof(PerObjectConstants));

	g_texture_sampler = RenderContext::createSampler(WrapMode::Clamp, FilterType::Linear);
	g_wrap_texture_sampler = RenderContext::createSampler(WrapMode::Repeat, FilterType::Linear);

	initGUIResources();
}

RenderFrame *Renderer::getNewRenderFrame() {
	frame_index = Math::wrap(++frame_index, (u32)0, (u32)1);
	RenderFrame* result = &frames[frame_index];
	result->sprites.clear();
	result->background_sprites.clear();
	result->gui_frame.items.clear();
	return result;
}

void Renderer::onResize(u32 width, u32 height) {

	RenderContext::setViewport(0, 0, (s32)width, (s32)height, 0.0f, 1.0f);
	RenderContext::resizeBuffer((s32)width, (s32)height);
}

void Renderer::processRenderFrame(RenderFrame *frame) {
	ProfileFunc;

	RenderContext::pushDebugGroup("Render Frame");
	
	Color clear_color = Color(0.20f, 0.60f, 0.51f, 1.0f);
	
	u32 window_width, window_height;
	Platform::getWindowSize(Platform::getCurrentWindow(), window_width, window_height);
	
	RenderContext::bindSampler(&g_texture_sampler, 0);
	RenderContext::bindSampler(&g_wrap_texture_sampler, 1);
	
	RenderContext::setBlendMode(BlendMode::AlphaBlend);
	RenderContext::setCullMode(CullMode::Front);
	
	PerSceneConstants scene_constants = {};
	Mat4 view_projection = frame->camera_projection * frame->camera_view;
	scene_constants.projection = view_projection;
	
	RenderContext::clear(clear_color);

	RenderContext::pushDebugGroup("Vector Render");
	{
		ProfileScope("nanoVG");
		extern NVGcontext* vg;
		nvgBeginFrame(vg, window_width, window_height, 1.0f);

		Vec2 camera_size = frame->camera_bounds.getSize();
		nvgScale(vg, (f32)window_width / camera_size.x, (f32)window_height / camera_size.y);
		nvgTranslate(vg, camera_size.x * 0.5f, camera_size.y * 0.5f);
		nvgScale(vg, 1.0f, -1.0f);
		nvgTranslate(vg, -frame->camera_position.x, -frame->camera_position.y);

		nvgBeginPath(vg);
		nvgCircle(vg, 0.0f, 0.0f, frame->game->arena_radius);
		
		nvgStrokeColor(vg, nvgRGB(104,32,121));
		nvgStroke(vg);

		nvgBeginPath(vg);
		nvgCircle(vg, 0.0f, 0.0f, frame->game->haven.radius);
		nvgFillColor(vg, nvgRGB(104,32,121));
		nvgFill(vg);

		frame->game->registry.view<Grenade, Position>().each([&](const entt::entity& entity, Grenade& grenade, Position& position) {
			nvgBeginPath(vg);
			nvgCircle(vg, position.position.x, position.position.y, grenade.radius);
			nvgFillColor(vg, nvgRGBA(3, 244, 252, 150));
			nvgFill(vg);
		});

		frame->game->registry.view<Position, Crack>().each([&](Position& pos, Crack& crack) {
			const Vec2 position = pos.position;
			nvgBeginPath(vg);
			nvgCircle(vg, position.x, position.y, 0.5f); 
			nvgFillColor(vg, nvgRGBAf(0.75f, 0.0f, 0.0f, 1.0f));
			nvgFill(vg);

			nvgBeginPath(vg);
			nvgMoveTo(vg, position.x, position.y);
			// nvgCircle(vg, position.x, position.y, 0.5f);
			Vec2 previous_point = position;
			if(crack.points.count > 0) {
				for(u32 i = 0; i < crack.points.count-1; i++) {
					Vec2 point = crack.points.getCopy(i);
					Vec2 dir = Vec2::normalize(point - previous_point);
					Vec2 l_perp = Vec2::lPerp(dir);
					Vec2 r_perp = Vec2::lPerp(dir);
					nvgLineTo(vg, point.x, point.y);
					previous_point = point;
				}
			}

			f32 d = crack.timer / crack.max_timer;
			Vec2 start_point = position;
			if(crack.points.count > 0) {
				if(crack.points.count > 1) {
					start_point = crack.points.getCopy(crack.points.count-2);
				}
				Vec2 point = crack.points.getCopy(crack.points.count-1);
				
				Vec2 end = Vec2::lerp(start_point, point, 1.0f - d);
				nvgLineTo(vg, end.x, end.y);
			}

			nvgLineCap(vg, NVG_ROUND);
			nvgStrokeColor(vg, nvgRGBAf(0.8f, 0.0f, 0.0f, 1.0f));
			nvgStroke(vg);

			
		});

		frame->game->registry.view<Position, EnemyHealth>().each([](Position& position, EnemyHealth& health) {
			
			f32 w = 2.0f;
			f32 health_w = (health.health / health.max_health) * w;
			nvgBeginPath(vg);
			nvgRect(vg, position.position.x - w * 0.5f, position.position.y + 1.0f, w, 0.2f);
			nvgFillColor(vg, nvgRGBAf(1.0f, 0.0f, 0.0f, 1.0f));
			nvgFill(vg);

			nvgBeginPath(vg);
			nvgRect(vg, position.position.x - w * 0.5f, position.position.y + 1.0f, health_w, 0.2f);
			nvgFillColor(vg, nvgRGBAf(0.0f, 1.0f, 0.0f, 1.0f));
			nvgFill(vg);
		});

		nvgEndFrame(vg);
	}
	RenderContext::popDebugGroup();
	
	RenderContext::bindShader(&Assets::shaders.sprite);
	RenderContext::bindVertexArray(&g_sprite_layout);
	
	TexturedVertex vertices[] = {
		{ Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f) },
		{ Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f) },
		{ Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f) },
		{ Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f) },
	};
	
	u32 vertex_count = ArrayCount(vertices);
	
	VertexBuffer vb = RenderContext::createVertexBuffer(vertices, sizeof(TexturedVertex), 4);
	
	RenderContext::bindVertexArray(&g_sprite_layout);
	RenderContext::bindVertexBufferToVertexArray(&g_sprite_layout, &vb);
	
	RenderContext::updateShaderConstant(&g_per_scene_constants, &scene_constants);
	RenderContext::bindShaderConstant(&g_per_scene_constants, 0);
	RenderContext::bindShaderConstant(&g_per_object_constants, 1);
	
	Texture2D* prev_bound_tex = 0;
	
	auto render_sprite = [&](const RenderSprite& sprite, const f32 z = 0.0f) {
		PerObjectConstants obj_data = {};
		Vec2 uv_offset = sprite.uv_offset;
		Vec2 uv_scale = sprite.uv_scale;
		
		if(sprite.flip_x) {
			uv_offset.x += uv_scale.x;
			uv_scale.x *= -1;
		}
	
		obj_data.uv_offset = uv_offset;
		obj_data.uv_scale = uv_scale;
		
		obj_data.color = sprite.color;
		obj_data.model = Mat4::translate(Vec3(sprite.position, 0.0f));
		if(sprite.rotation != 0.0f) {
			Vec3 pivot = Vec3(sprite.size * 0.5f, 0.0f);
			obj_data.model *= Mat4::translate(pivot);
			obj_data.model *= Mat4::rotateZ(sprite.rotation);
			obj_data.model *= Mat4::translate(-pivot);

		}
		obj_data.model *= Mat4::scale(Vec3(sprite.size, 1.0f));
		// DebugRenderQueue::addAABB(uv_offset, uv_offset + Vec2(2.0f));
		RenderContext::updateShaderConstant(&g_per_object_constants, &obj_data);
		if(prev_bound_tex != sprite.sheet) {
			RenderContext::bindTexture2D(sprite.sheet, 0);
			prev_bound_tex = sprite.sheet;
		}
		
		RenderContext::sendDraw(Topology::TriangleStrip, vertex_count);
	
	};
	
	RenderContext::setDepthMode(DepthMode::Disabled);
	RenderContext::setMultisampleEnabled(false);
	RenderContext::setAlphaToCoverageEnabled(false);
	
	{
		ProfileScope("renderBackgroundSprites");
		RenderContext::pushDebugGroup("Render Background Sprites");
		for(const RenderSprite& sprite : frame->background_sprites) {
			render_sprite(sprite);
		}
		RenderContext::popDebugGroup();
	}
	
	{
		ProfileScope("cullSprites");
		for(s32 i = frame->sprites.count-1; i >= 0; i--) {
			const RenderSprite& sprite = frame->sprites.getRefConst(i);
			const AABB sprite_bounds = sprite.getAABB();
			if(!sprite_bounds.intersects(frame->camera_bounds)) {
				frame->sprites.removeAtIndexUnordered(i);
			}
		}
	}
	
	// RenderContext::setDepthMode(DepthMode::LEqual);
	// RenderContext::setMultisampleEnabled(true);
	// RenderContext::setAlphaToCoverageEnabled(true);
	
	
	
	// DebugRenderQueue::addLine(Vec2(), Vec2(1.0f, 0.0f), Colors::red);
	// DebugRenderQueue::addLine(Vec2(), Vec2(0.0f, 1.0f), Colors::green);
	
	// RenderContext::setMultisampleEnabled(false);
	// RenderContext::setAlphaToCoverageEnabled(false);

	

	{
		ProfileScope("sortSprites");
		std::sort(frame->sprites.data, frame->sprites.data + frame->sprites.count, [](const RenderSprite& a, const RenderSprite& b) {
			return a.position.y > b.position.y;
		});
	}
	
	{
		ProfileScope("renderSprites");
		RenderContext::pushDebugGroup("Render Sprites");
		for(const RenderSprite& sprite : frame->sprites) {
	
			render_sprite(sprite, sprite.position.y);
			// DebugRenderQueue::addAABB(sprite.position, sprite.position + sprite.size);
		}
		RenderContext::popDebugGroup();
	}
	
	
	
	RenderContext::destroyVertexBuffer(&vb);
	
	DebugRenderQueue::flushRender(&Assets::shaders.editor, view_projection);
	
	renderGUIFrame(frame->gui_frame);
	
	// ============= IMGUI ================================================================
	
	{
		ProfileScope("ImGui::Render");;
		ImGui::Render();
		ImGuiImpl::render();
	}

	{
		ProfileScope("present");
		RenderContext::present();
	}

	RenderContext::popDebugGroup();
}
