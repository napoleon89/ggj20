#include "profiler.h"
#include <core/math.h>
#include <imgui/imgui.h>

global_variable u32 frame_index = 0;

LocalTimer::LocalTimer(const char *function_name) {
	timer.start();
	profiler_entry_index = Profiler::startProfilingFunction(function_name);
	
}

LocalTimer::~LocalTimer() {
	float tookMs = timer.getMillisecondsElapsed();
	Profiler::finishProfilingFunction(profiler_entry_index, tookMs);
}

int Profiler::startProfilingFunction(const char* function_name) {
	if (paused) return -1;
	ProfilerFrame &frame = frames[current_frame_index];
	ProfilerEntry entry = {};
	entry.function_name = function_name;
	entry.stack_depth = stack_depth;
	entry.start_time = frame_timer.getMillisecondsElapsed();
	frame.entries.add(entry);
	stack_depth++;
	return frame.entries.count-1;
}

void Profiler::finishProfilingFunction(int entry_index, f32 time_taken_ms) {
	if (paused) return;
	ProfilerFrame &frame = frames[current_frame_index];
	ProfilerEntry &entry = frame.entries.getRef(entry_index);
	entry.time_taken_ms = time_taken_ms;
	stack_depth--;
	
}

void Profiler::finishFrame() {
	if (paused) return;
	frame_index++;
	current_frame_index = Math::wrap(++current_frame_index, 0, PROFILER_FRAME_BUFFER_COUNT-1);
	ProfilerFrame &frame = frames[current_frame_index];
	frame.entries.clear();
	frame.frame_index = frame_index;

	frame.total_time = frame_timer.getMillisecondsElapsed();
}

void Profiler::startFrame() {
	if(paused) return;
	frame_timer.start();
}

void Profiler::editorGui(bool* open) {
	ImGui::Begin("Profiler", open); {
		int previous_frame_index = Math::wrap(current_frame_index-1, 0, PROFILER_FRAME_BUFFER_COUNT - 1);
		//ImGui::BeginCombo("Frame", 0);
		//for (s32 i = 0; i < PROFILER_FRAME_BUFFER_COUNT; i++) {
		//	s32 frame_index = Math::wrap(current_frame_index + i, 0, PROFILER_FRAME_BUFFER_COUNT-1);
		//	const ProfilerFrame &frame = frames[frame_index];
		//	ImGui::Selectable()
		//}
		//ImGui::EndCombo();

		static float graph_scale = 1.0f;
		ImGui::DragFloat("Graph Scale", &graph_scale, 0.1f, 0.0f);

		const float window_width = ImGui::GetContentRegionAvailWidth();

		const ProfilerFrame &frame = frames[previous_frame_index];

	    ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
		
		f32 padding = 10.0f;
		p.x += padding;
		p.y += padding;
		const float full_width = window_width - padding * 2.0f;
		const float row_height = 20.0f;
		const float ms_unit = full_width / frame.total_time;
		
		draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + full_width, p.y + row_height), ImColor(1.0f, 0.0f, 0.0f, 1.0f));
		// draw_list->AddRectFilled(ImVec2(p.x, p.y + row_height), ImVec2(p.x + ms_unit , p.y + row_height*2.0f), ImColor(0.0f, 1.0f, 0.0f, 1.0f));
		
		for (u32 i = 0; i < frame.entries.count; i++) {
			const ProfilerEntry &entry = frame.entries.getRefConst(i);
			f32 walking_time = entry.start_time * ms_unit;
			f32 time = entry.time_taken_ms * ms_unit;
			f32 row_offset = row_height * (entry.stack_depth+1) + stack_depth;
			
			ImVec2 start_pos = ImVec2(p.x + walking_time, p.y + row_offset);
			ImVec2 end_pos = ImVec2(p.x + walking_time + time, p.y + row_offset + row_height);

			if(ImGui::IsMouseHoveringRect(start_pos, end_pos)) {
				ImGui::SetTooltip("%s: %.3fms", entry.function_name, entry.time_taken_ms);
			}

			ImColor color = ImColor::HSV(((f32)i / (f32)frame.entries.count) * 360.0f, 1.0f, 1.0f);
			
			draw_list->AddRectFilled(start_pos, end_pos, color);
			// draw_list->AddRect(start_pos, end_pos, ImColor(0.0f, 0.0f, 0.0f, 1.0f));
			// draw_list->PushClipRect(start_pos, end_pos);
			// draw_list->AddText(start_pos, ImColor(0.0f, 0.0f, 0.0f, 1.0f), entry.function_name);
		}
		
		ImGui::SetCursorPosY(row_height * 10.0f);
		
		u32 stack_depth = 0;
		for (u32 i = 0; i < frame.entries.count; i++) {
			const ProfilerEntry &entry = frame.entries.getRefConst(i);
			for(u32 t = 0; t < entry.stack_depth; t++) {
				ImGui::Indent();
			}
			ImGui::Text("%s: %.3fms - %.3fms", entry.function_name, entry.time_taken_ms, entry.start_time);
			for(u32 t = 0; t < entry.stack_depth; t++) {
				ImGui::Unindent();
			}
		}
	} ImGui::End();
}
