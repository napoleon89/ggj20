#pragma once

#include <functional>
#include <core/collections/dynamic_array.h>
#include <core/math.h>

struct GUIFrame;

enum class PositionType {
	RelativeUnits,
	RelativePixels,
};

enum class SizeType {
	Relative,
	Absolute,
};

enum class HorizontalTextAlign {
	Left,
	Center,
	Right,
};

enum class VerticalTextAlign {
	Top,
	Center,
	Bottom,
};

struct ElementLayoutData {
	PositionType x_type = PositionType::RelativeUnits;
	PositionType y_type = PositionType::RelativeUnits;
	float x = 0.0f;
	float y = 0.0f;

	SizeType width_type = SizeType::Relative;
	SizeType height_type = SizeType::Relative;
	float width = 1.0f;
	float height = 1.0f;

	float pivot_x = 0.0f;
	float pivot_y = 0.0f;

	float padding_pixels = 0.0f;
};

struct RenderBox {
	Vec2 offset;
	Vec2 size;
};

struct Element {
	ElementLayoutData layout_data;
	DynamicArray<Element*> children;
	RenderBox scratch_render_box;
	Color color = Colors::transparent;
	Color text_color = Colors::black;
	bool visible = true;

	std::string text;
	f32 font_size = 16.0f;
	f32 border_radius;
	

	HorizontalTextAlign text_align_hor;
	VerticalTextAlign text_align_ver;

	std::function<void()> on_click;
	
	virtual void layoutChildren();
	virtual void onHoverEnter();
	virtual void onHoverLeave();

	template<typename T>
	T* addChild() {
		T* child = new T();
		children.add(child);
		return child;
	}

	// don't touch me 
	bool hovered;
};

struct GridElement : public Element {
	u32 row_count = 1;
	u32 column_count = 1;

	virtual void layoutChildren() override;
};

struct ButtonElement : public Element {
	Color hover_color;
	
	virtual void onHoverEnter() override;
	virtual void onHoverLeave() override;
};

struct Constraint {
	Vec2 min, max;
};

struct ElementLayoutTree {
	Element root;

	void processLayout();
	void processPaint(GUIFrame *frame);
	
};

Vec2 layoutElementSizes(Element *element, const Constraint parent_constraint);