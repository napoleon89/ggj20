#include <game/editor.h>
#include <engine/input.h>
#include <core/render_context.h>
#include <game/assets.h>
#include <game/game.h>
#include <imgui/imgui.h>
#include <engine/debug_renderer.h>
#include <engine/profiler.h>
#include <engine/json.h>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

struct AnimKey {
	f32 time;
	f32 value;
	f32 influence_in;
	f32 influence_out;
};

struct Anim {
	DynamicArray<AnimKey> keys;
	f32 start_value;
	f32 end_value;
	f32 end_time;
	std::string name;

	f32 getInfluenceIn(const s32 index) const {
		const u32 wrapped_index = (u32)Math::wrap(index, 0, (s32)keys.count);
		return keys.getRefConst(wrapped_index).influence_in;
	}

	f32 getInfluenceOut(const s32 index) const {
		const u32 wrapped_index = (u32)Math::wrap(index, 0, (s32)keys.count);
		return keys.getRefConst(wrapped_index).influence_out;
	}

	Vec2 getValueAtTime(const f32 time, const f32 x_unit, const f32 y_unit, const u32 min_value, const u32 range) const {
		const f32 wrapped_time = Math::wrap(time, 0.0f, end_time);
		u32 start_frame, end_frame;
		for(u32 i = 0; i < keys.count-1; i++) {
			const AnimKey& key = keys.getRefConst(i);
			const AnimKey& next_key = keys.getRefConst(i+1);
			if(time >= key.time && time < next_key.time) {
				start_frame = i;
				end_frame = i+1;
				break;
			}
		}

		const AnimKey& key = keys.getRefConst(start_frame);
		const AnimKey& next_key = keys.getRefConst(end_frame);
		
		const Vec2 pos = Vec2(key.time, key.value);
		const Vec2 next_pos = Vec2(next_key.time, next_key.value);
		
		// const Vec2 pos = Vec2(key.time * x_unit, (range - (key.value - min_value)) * y_unit);
		// const Vec2 next_pos = Vec2(next_key.time * x_unit, (range - (next_key.value - min_value)) * y_unit);

		const f32 width = next_pos.x - pos.x;
		const f32 influence_in = (getInfluenceOut(start_frame-1) / 100.0f) * width;
		const f32 influence_out = (getInfluenceIn(end_frame) / 100.0f) * width;	
		
		const Vec2 bez_start = pos;
		const Vec2 bez_end = next_pos;
		const Vec2 bez_c1 = Vec2(bez_start.x + influence_in, bez_start.y);
		const Vec2 bez_c2 = Vec2(bez_end.x - influence_out, bez_end.y);

		const f32 t = (time - key.time) / (next_key.time - key.time);
		Vec2 result = Math::cubCurve(bez_start, bez_c1, bez_c2, bez_end, t);
		return result;
	}
};

DynamicArray<Anim> g_anims;

void EditorState::init() {
	enabled = false;
	was_enabled = false;

	// FileData file_data = Platform::readEntireFile("data/opitest.DuAEF");
	// JsonDocument document = {};
	// document.parseFromMemory(file_data.contents, file_data.size);
	// const JsonObject& obj = document.root.getObject();
	// const JsonArray& layers = obj.search("layers")->getArray();
	// const JsonObject& layer = layers.getRefConst(0).getObject();
	// const std::string& name = layer.search("_name")->getString();
	// const JsonArray& anims = layer.search("anims")->getArray();
	// const JsonArray& actual_anims = anims.getRefConst(0).getObject().search("anims")->getArray();
	// const JsonObject& y_anim = actual_anims.getRefConst(1).getObject();
	// const JsonArray& keys = y_anim.search("keys")->getArray();
	
	// anim.start_value = y_anim.search("startValue")->getNumber();
	// anim.end_value = y_anim.search("endValue")->getNumber();
	// anim.end_time = y_anim.search("endTime")->getNumber();

	// anim.keys.reserve(keys.count);

	// for(u32 i = 0; i < keys.count; i++) {
	// 	const JsonObject& json_key = keys.getRefConst(i).getObject();
	// 	const JsonObject& in_ease = json_key.search("inEase")->getArray().getRefConst(0).getObject();
	// 	const JsonObject& out_ease = json_key.search("outEase")->getArray().getRefConst(0).getObject();
		
	// 	AnimKey key = {};
	// 	key.time = json_key.search("_time")->getNumber();
	// 	key.value = json_key.search("value")->getNumber();
	// 	key.influence_in = in_ease.search("influence")->getNumber();
	// 	key.influence_out = out_ease.search("influence")->getNumber();
	// 	anim.keys.add(key);
	// }
}

void EditorState::onShow(GameState *g) {
	game = g;
	start_camera = game->camera;
	
}

void EditorState::onHide() {
	game->camera = start_camera;
}

void EditorState::update(f32 delta, PlatformWindow *window) {
	timer += delta;

	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Tools")) {
			ImGui::MenuItem("Profiler", NULL, &show_profiler);
			ImGui::EndMenu();
		}

		
		ImGui::EndMainMenuBar();	
	} 
	anim_machine.progress(delta);
	
	// ImGui::DockSpaceOverViewport();

	f32 e_width = 250.0f;
	f32 margin = 5.0f;
	// ImGui::SetNextWindowPos(ImVec2(game->camera.window_dimensions.x - (e_width + margin), 25.0f));

	if(show_profiler) {
		Profiler::editorGui(&show_profiler);
	}

	// ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
	// ImGui::SetNextWindowSize(ImVec2(1200, 700), ImGuiCond_FirstUseEver);
	// ImGui::Begin("Anim window");
	//
	// const float window_width = ImGui::GetContentRegionAvailWidth();
	// f32 padding = 0.0f;
	//
	// const float full_width = window_width - padding * 2.0f;
	//
	// for(u32 anim_index = 0; anim_index < g_anims.count; anim_index++) {
	// 	const Anim& anim = g_anims.getRefConst(anim_index);
	//
	// 	const float x_unit = full_width / anim.end_time;
	//
	// 	f32 max_value = 0.0f;
	// 	f32 min_value = FLT_MAX;
	// 	for(u32 i = 0; i < anim.keys.count; i++) {
	// 		f32 value = anim.keys.getRefConst(i).value;
	// 		max_value = Math::max(value, max_value);
	// 		min_value = Math::min(value, min_value);
	// 	}
	//
	// 	f32 range = max_value - min_value;
	//
	// 	
	// 	
	// 	// f32* plot_values = (f32*)anim.keys.data;
	// 	//
	// 	// ImGui::PlotLines(
	// 	// 	"##AnimCurve", 
	// 	// 	plot_values, 
	// 	// 	anim.keys.count, 
	// 	// 	0, NULL, FLT_MAX, FLT_MAX, 
	// 	// 	ImVec2(full_width, 160)
	// 	// );
	// 	ImGui::Text(anim.name.c_str());
	// 	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.34f, 0.34f, 0.34f, 1.0f));
	// 	ImVec2 child_size = ImVec2(full_width, 160);
	// 	ImGui::BeginChild(("##AnimCurve" + std::to_string(anim_index)).c_str(), child_size, false);
	// 	{
	// 		const float y_unit = child_size.y / range;
	//
	// 		ImVec2 p = ImGui::GetCursorScreenPos();
	// 		ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//
	// 		for(u32 i = 0; i < anim.keys.count-1; i++) {
	// 			const AnimKey& key = anim.keys.getRefConst(i);
	// 			const AnimKey& next_key = anim.keys.getRefConst(i+1);
	// 			const ImVec2 pos = p + ImVec2(key.time * x_unit, (range - (key.value - min_value)) * y_unit);
	// 			const ImVec2 next_pos = p + ImVec2(next_key.time * x_unit, (range - (next_key.value - min_value)) * y_unit);
	//
	// 			const f32 width = next_pos.x - pos.x;
	// 			const f32 influence_in = (anim.getInfluenceOut(i-1) / 100.0f) * width;
	// 			const f32 influence_out = (anim.getInfluenceIn(i+1) / 100.0f) * width;	
	//
	// 			
	// 			const ImVec2 bez_start = pos;
	// 			const ImVec2 bez_end = next_pos;
	// 			const ImVec2 bez_c1 = ImVec2(bez_start.x + influence_in, bez_start.y);
	// 			const ImVec2 bez_c2 = ImVec2(bez_end.x - influence_out, bez_end.y);
	//
	// 			draw_list->AddBezierCurve(
	// 				bez_start, 
	// 				bez_c1, 
	// 				bez_c2, 
	// 				bez_end, 
	// 				ImColor(1.0f, 0.0f, 0.0f, 1.0f), 2.0f, 
	// 				100
	// 			);
	//
	// 			draw_list->AddCircleFilled(pos, 5.0f, ImColor(1.0f, 1.0f, 0.0f));
	// 		}
	// 		
	// 		const u32 tess = 100;
	// 		for(f32 t = 0.0f; t < anim.end_time; t += anim.end_time / (f32)tess) {
	// 			f32 time = t;
	// 			Vec2 value = anim.getValueAtTime(time, x_unit, y_unit, min_value, range);
	// 			// const ImVec2 pos = p + ImVec2(value.x, value.y);
	// 			const ImVec2 pos = p + ImVec2(value.x * x_unit, (range - (value.y - min_value)) * y_unit);
	// 			draw_list->AddCircleFilled(pos, 2.5f, ImColor(1.0f, 1.0f, 0.0f, 1.0f));
	//
	// 			// DebugRenderQueue::addCircle(Vec2(time * 100.0f, (value - 200) * 10.0f), 0.5f);
	// 			
	// 		}
	// 		
	// 	}
	// 	ImGui::EndChild();
	// 	ImGui::PopStyleColor();
	//
	// 	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//     ImVec2 p = ImGui::GetCursorScreenPos();
	// 	p.x += padding;
	// 	p.y += padding;
	// 	
	// 	const float row_height = 20.0f;
	// 	
	// 	draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + full_width, p.y + row_height), ImColor(0.7f, 0.7f, 0.7f, 1.0f));
	//
	// 	f32 y = p.y + row_height * 0.5f;
	// 	for(u32 i = 0; i < anim.keys.count; i++) {
	// 		const AnimKey& key = anim.keys.getRefConst(i);
	// 		f32 x = p.x + x_unit * key.time;
	// 		f32 radius = row_height * 0.25f;
	// 		ImVec2 pos = ImVec2(x, y);
	// 		ImVec2 min = ImVec2(x - radius, y - radius);
	// 		ImVec2 max = ImVec2(x + radius, y + radius);
	// 		draw_list->AddCircleFilled(pos, radius, ImColor(0.75f, 0.0f, 0.0f, 1.0f));
	// 		if(ImGui::IsMouseHoveringRect(min, max)) {
	//             ImGui::BeginTooltip();
	// 			ImGui::Text("Time: %.6f", key.time);
	// 			ImGui::Text("Value: %.6f", key.value);
	// 			ImGui::EndTooltip();
	// 		}
	// 	}
	// 	
	// 	ImGui::SetCursorPosY(ImGui::GetCursorPos().y + row_height + 10);
	// }
	//
	// Vec2 uv_offset = anim_machine.current_animation->getFrameUVOffset(anim_machine.frame_index);
	// Vec2 uv_scale = anim_machine.current_animation->getFrameUVScale();
	// Vec2 frame_size = anim_machine.current_animation->frame_size;
	//
	// ImVec2 anim_start_point = ImGui::GetCursorScreenPos();
	//
	// ImGui::Image(
	// 	(ImTextureID)&anim_machine.current_animation->sheet, 
	// 	ImVec2(frame_size.x, frame_size.y),
	// 	ImVec2(uv_offset.x, uv_offset.y),
	// 	ImVec2(uv_offset.x, uv_offset.y) + ImVec2(uv_scale.x, uv_scale.y)
	// );
	//
	// ImDrawList* draw_list = ImGui::GetWindowDrawList();
	//
	// const Anim& y_anim = g_anims.getRefConst(1);
	// for(u32 i = 0; i < y_anim.keys.count; i++) {
	// 	const AnimKey& key = y_anim.keys.getRefConst(i);
	// 	ImVec2 point = anim_start_point + ImVec2(frame_size.x * 0.5f, key.value);
	// 	draw_list->AddCircleFilled(point, 5.0f, ImColor(0.0f, 0.0f, 0.0f, 1.0f));
	// }
	//
	// ImGui::End();
	// ImGui::PopStyleVar();
	
	// ImGui::SetNextWindowBgAlpha(0.0f);
	// ImGui::Begin("Game view"); {
	// 	// ImGui::Image((ImTextureID)&game->hdr_color_render_texture.texture, ImVec2(800, 450));
	// 	ImVec2 window_pos = ImGui::GetWindowPos();
	// 	ImVec2 window_size = ImGui::GetWindowSize();
	// 	// RenderContext::setViewport(window_pos.x, window_pos.y, window_size.x, window_size.y, 0.0f, 1.0f);
	// 	ImGui::End();
	// }
	
	

	/*DebugRenderQueue::addLine(Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	DebugRenderQueue::addLine(Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec4(0.0f, 1.0f, 0.0f, 1.0f));*/

		
}

void EditorState::render(PlatformWindow *window, f32 delta) {
	
}
