#include <core/render_context.h>
#include <core/collections/dynamic_array.h>

#include <engine/debug_renderer.h>
#include <engine/profiler.h>


struct DebugRenderLine {
	Color color;
	Vec2 start;
	Vec2 end;
	f32 thickness;
};

struct DebugRenderAABB {
	Vec2 min, max;
	Color color;
	f32 thickness;
};

struct DebugRenderCircle {
	Vec2 center;
	f32 radius;
	Color color;
	f32 thickness;
};

struct DebugPerScene {
	Mat4 view_projection;
};

struct DebugPerObject {
	Mat4 model;
	Color color;
};

struct PerInstanceData {
	Vec2 position;
	Vec2 scale;
	f32 angle;
	Color color;
};

global_variable std::vector<DebugRenderLine> g_debug_lines;
global_variable std::vector<DebugRenderAABB> g_debug_aabbs;
global_variable std::vector<DebugRenderCircle> g_debug_circles;
global_variable ShaderConstant g_per_scene_constant;
global_variable ShaderConstant g_per_object_constant;
global_variable VertexArray g_debug_shader_layout;

void DebugRenderQueue::init(Shader *debug_shader) {
	g_debug_shader_layout = RenderContext::createVertexArray({
		{"VERT_POSITION", RenderContext::Format::Vec2},
		{"INSTANCE_POSITION", RenderContext::Format::Vec2, 1, 1},
		{"INSTANCE_SCALE", RenderContext::Format::Vec2, 1, 1},
		{"INSTANCE_ANGLE", RenderContext::Format::F32, 1, 1},
		{"INSTANCE_COLOR", RenderContext::Format::Vec4, 1, 1},
		
	});
	g_per_scene_constant = RenderContext::createShaderConstant(sizeof(DebugPerScene));
	g_per_object_constant = RenderContext::createShaderConstant(sizeof(DebugPerObject));

}

void DebugRenderQueue::addLine(const Vec2& start, const Vec2& end, const Color& color, f32 thickness) {
	DebugRenderLine line = {};
	line.start = start;
	line.end = end;
	line.color = color;
	line.thickness = thickness;
	g_debug_lines.push_back(line);
}

void DebugRenderQueue::addAABB(const Vec2& min, const Vec2& max, const Color& color, f32 thickness) {
	DebugRenderAABB aabb = {};
	aabb.min = min;
	aabb.max = max;
	for (u32 i = 0; i < 1; i++) {
		if (aabb.min.xy[i] > aabb.max.xy[i])
			Swap(aabb.min.xy[i], aabb.max.xy[i]);
	}
	aabb.color = color;
	aabb.thickness = thickness;
	g_debug_aabbs.push_back(aabb);
}

void DebugRenderQueue::addCircle(const Vec2& center, const f32 radius, const Color& color, f32 thickness) {
	DebugRenderCircle circle = {center, radius, color, thickness};
	g_debug_circles.push_back(circle);
}

void DebugRenderQueue::startframe() {
	ProfileFunc;
	g_debug_lines.clear();
	g_debug_aabbs.clear();
	g_debug_circles.clear();
}

void DebugRenderQueue::flushRender(Shader *debug_shader, const Mat4 &view_projection) {
	ProfileFunc;
	RenderContext::pushDebugGroup("Render Debug Geom");
	
	for (size_t i = 0; i < g_debug_aabbs.size(); i++) {
		const DebugRenderAABB &handle = g_debug_aabbs[i];
		// drawLineHandle(handle.min, handle.max, handle.color);
		Vec2 bottom_left = Vec2(handle.min.x, handle.min.y);
		Vec2 bottom_right = Vec2(handle.max.x, handle.min.y);
		Vec2 top_left = Vec2(handle.min.x, handle.max.y);
		Vec2 top_right = Vec2(handle.max.x, handle.max.y);

		addLine(bottom_left, top_left, handle.color, handle.thickness);
		addLine(top_left, top_right, handle.color, handle.thickness);
		addLine(top_right, bottom_right, handle.color, handle.thickness);
		addLine(bottom_right, bottom_left, handle.color, handle.thickness);
	}

	for (size_t i = 0; i < g_debug_circles.size(); i++) {
		const DebugRenderCircle& circle = g_debug_circles[i];
		const u32 tesselation = 24;

		
		f32 d = Math::Pi/(tesselation * 0.5f);
		Vec2 previous_pos = Vec2(Math::cos(0.0f) * circle.radius, Math::sin(0.0f) * circle.radius) + circle.center;
		
		for(u32 c = 1; c <= tesselation; c++) {
			f32 t = d * c;
			Vec2 pos = Vec2(Math::cos(t) * circle.radius, Math::sin(t) * circle.radius) + circle.center;
			addLine(previous_pos, pos, circle.color, circle.thickness);
			previous_pos = pos;
		}
	}

	RenderContext::bindShader(debug_shader);
	{
		Vec2 vertices[] = {
			Vec2(-0.5f, 0.5f),
			Vec2(0.5f, 0.5f),
			Vec2(-0.5f, -0.5f),
			Vec2(0.5f, -0.5f)
		};

		DebugPerScene per_scene_data = {};
		per_scene_data.view_projection = view_projection;
		RenderContext::updateShaderConstant(&g_per_scene_constant, &per_scene_data);
		RenderContext::bindShaderConstant(&g_per_scene_constant, 0);

		RenderContext::bindShaderConstant(&g_per_object_constant, 1);
		RenderContext::bindVertexArray(&g_debug_shader_layout);
		u32 vertex_count = ArrayCount(vertices);
		
		
		VertexBuffer vb = RenderContext::createVertexBuffer(vertices, sizeof(Vec2), vertex_count);
		
		RenderContext::bindVertexBufferToVertexArray(&g_debug_shader_layout, &vb);
		// DebugPerObject per_object = {};
		// per_object.color = Colors::red;
		// per_object.model = Mat4();
		// RenderContext::updateShaderConstant(&g_per_object_constant, &per_object);

		
		
		
		const u32 instance_count = g_debug_lines.size();
		PerInstanceData* instance_data = (PerInstanceData*)Memory_alloc(instance_count * sizeof(PerInstanceData));
		
		{
			ProfileScope("prepareInstanceData");
			for (size_t i = 0; i < g_debug_lines.size(); i++) {
				DebugRenderLine handle = g_debug_lines[i];
				// for(u32 i = 0; i < 2; i++) {
				// 	if(handle.end.xy[i] < handle.start.xy[i])
				// 		Swap(handle.end.xy[i], handle.start.xy[i]);
				// }
				Vec2 dir = handle.end - handle.start;
				PerInstanceData data = {};	 	
				
				Vec2 dir_norm = Vec2::normalize(dir);

				data.angle = Vec2::angle(Vec2::up(), dir_norm);;
				data.position = handle.start + dir * 0.5f;
				data.scale = Vec2(handle.thickness, Vec2::length(dir));
				data.color = handle.color;
				instance_data[i] = data;
			}
		}
		VertexBuffer instance_buffer = RenderContext::createVertexBuffer(instance_data, sizeof(PerInstanceData), instance_count);
		RenderContext::bindVertexBufferToVertexArray(&g_debug_shader_layout, &instance_buffer, 1);
		{
			ProfileScope("submitData");	
			RenderContext::sendDrawInstanced(Topology::TriangleStrip, vertex_count, instance_count);
		}

		Platform::free(instance_data);
		RenderContext::destroyVertexBuffer(&instance_buffer);
		RenderContext::destroyVertexBuffer(&vb);
	}

	RenderContext::popDebugGroup();
}