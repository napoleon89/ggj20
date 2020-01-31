#include <core/math.h>
#include <core/render_context.h>

#include <engine/gui_renderer.h>
#include <engine/profiler.h>

#include <game/assets.h>

#include <nanovg/nanovg.h>

global_variable int g_font;

void initGUIResources() {
	extern NVGcontext* vg;
	g_font = nvgCreateFont(vg, "Roboto-Regular", "data/fonts/Roboto-Regular.ttf");
}

void renderGUIFrame(const GUIFrame& frame) {
	ProfileFunc;
	RenderContext::pushDebugGroup("Gui Render");
	
	extern NVGcontext* vg;
	
	u32 window_width, window_height;
	Platform::getWindowSize(Platform::getCurrentWindow(), window_width, window_height);
	
	nvgBeginFrame(vg, window_width, window_height, 1.0f);
	
	for(const GUIBox &box : frame.items) {
		// fillRect(box.position, box.size, box.color);
		nvgBeginPath(vg);
		nvgRoundedRect(vg, box.position.x, box.position.y, box.size.x, box.size.y, box.border_radius);
		nvgFillColor(vg, nvgRGBAf(box.color.r, box.color.g, box.color.b, box.color.a));
		nvgFill(vg);

		if(!box.text.empty()) {
			nvgFontFaceId(vg, g_font);
			nvgFontSize(vg, box.font_size);

			f32 text_x = box.position.x;
			f32 text_y = box.position.y;
			
			float bounds[4];
			nvgTextBoxBounds(vg, box.position.x, box.position.y, box.size.x, box.text.c_str(), nullptr, bounds);
			f32 text_height = bounds[3] - bounds[1];
			
			int x_text_align = 1 << (int)box.text_align_hor;
			int y_text_align = 1 << ((int)box.text_align_ver + 3);

			switch(box.text_align_ver) {
				case VerticalTextAlign::Top: { } break;
				
				case VerticalTextAlign::Center: {
					text_y = box.position.y + box.size.y * 0.5f;
				} break;
				
				case VerticalTextAlign::Bottom: {
					
				} break;
			}
			
			nvgFillColor(vg, nvgRGBAf(box.text_color.r, box.text_color.g, box.text_color.b, box.text_color.a));
			nvgTextAlign(vg, x_text_align | y_text_align);
			nvgTextBox(vg, text_x, text_y, box.size.x, box.text.c_str(), nullptr);
		}
	}

	nvgEndFrame(vg);

	RenderContext::popDebugGroup();
}
