#pragma once


#include <core/math.h>
#include <core/collections/dynamic_array.h>

#include <engine/gui/gui.h>

struct GUIBox {
	Vec2 position;
	Vec2 size;
	Color color;
	Color text_color;
	std::string text;
	f32 font_size;
	f32 border_radius;

	HorizontalTextAlign text_align_hor;
	VerticalTextAlign text_align_ver;
};

struct GUIFrame {
	DynamicArray<GUIBox> items;
};

void initGUIResources();
void renderGUIFrame(const GUIFrame& frame);