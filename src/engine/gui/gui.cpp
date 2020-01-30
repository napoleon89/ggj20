#include <engine/gui/gui.h>
#include <engine/gui_renderer.h>
#include <engine/input.h>
#include <engine/physics.h>

Vec2 layoutElementSizes(Element *element, const Constraint parent_constraint) {
	// calculate element size from parent_constraints

	Vec2 new_size = Vec2();

	switch (element->layout_data.width_type) {
		case SizeType::Absolute: new_size.x = element->layout_data.width; break;
		case SizeType::Relative:  new_size.x = parent_constraint.max.x * element->layout_data.width; break;
	}

	switch (element->layout_data.height_type) {
		case SizeType::Absolute: new_size.y = element->layout_data.height; break;
		case SizeType::Relative:  new_size.y = parent_constraint.max.y * element->layout_data.height; break;
	}
	
	new_size.x = Math::clamp(new_size.x, parent_constraint.min.x, parent_constraint.max.x);
	new_size.y = Math::clamp(new_size.y, parent_constraint.min.y, parent_constraint.max.y);
	
	element->scratch_render_box.size = new_size;
	
	element->layoutChildren();

	return new_size;
}



void Element::layoutChildren() {
	
	Constraint constraint = { Vec2(), scratch_render_box.size - layout_data.padding_pixels * 2.0f };

	for (u32 i = 0; i < children.count; i++) {
		Element *child = children.getCopy(i);
		Vec2 child_size = layoutElementSizes(child, constraint);
	}
}

void Element::onHoverEnter() { }
void Element::onHoverLeave() { }

void ButtonElement::onHoverEnter() {
	Swap(hover_color, color);
}

void ButtonElement::onHoverLeave() {
	Swap(hover_color, color);
}


void GridElement::layoutChildren() {
	Constraint constraint = {};

	u32 row_index = 0;
	u32 column_index = 0;

	constraint.max = scratch_render_box.size / Vec2(column_count, row_count);
	
	for (u32 i = 0; i < children.count; i++) {
	 	Element *child = children.getCopy(i);
	 	Vec2 child_size = layoutElementSizes(child, constraint);

		
	 	child->scratch_render_box.offset = constraint.max * Vec2(column_index, row_index);
		
		column_index++;
		if(column_index == column_count) {
			column_index = 0;
			row_index++;
		}
	}
}

void ElementLayoutTree::processLayout() {
	u32 window_width, window_height;
	Platform::getWindowSize(Platform::getCurrentWindow(), window_width, window_height);
	Vec2 window_size = Vec2(window_width, window_height);

	Constraint root_constraint = { window_size, window_size };
	Vec2 root_box = layoutElementSizes(&root, root_constraint);
}

static void paintElement(Element *element, const Vec2 parent_position, const Vec2 parent_size, const bool left_clicked, Vec2 mouse_pos, GUIFrame *frame) {
	
	Vec2 new_position = Vec2();
	switch(element->layout_data.x_type) {
		case PositionType::RelativePixels: new_position.x = parent_position.x + element->layout_data.x; break;
		case PositionType::RelativeUnits: new_position.x = parent_position.x + (element->layout_data.x * parent_size.x); break;
	}

	
	switch (element->layout_data.y_type) {
		case PositionType::RelativePixels: new_position.y = parent_position.y + element->layout_data.y; break;
		case PositionType::RelativeUnits: new_position.y = parent_position.y + (element->layout_data.y * parent_size.y); break;
	}

	new_position.x -= element->layout_data.pivot_x * element->scratch_render_box.size.x;
	new_position.y -= element->layout_data.pivot_y * element->scratch_render_box.size.y;
	
	new_position += element->layout_data.padding_pixels;
	
	new_position += element->scratch_render_box.offset;

	AABB bounds {new_position, new_position + element->scratch_render_box.size};
	if(bounds.pointInside(mouse_pos)) {
		if(!element->hovered) {
			element->hovered = true;
			element->onHoverEnter();
		}
	} else if(element->hovered) {
		element->hovered = false;
		element->onHoverLeave();
	}

	if(element->hovered && left_clicked && element->on_click != nullptr) {
		element->on_click();
	}

	if(element->color.a > 0.0f && element->visible) {
		GUIBox box = {
			new_position,
			element->scratch_render_box.size,
			element->color,
			element->text_color,
			element->text,
			element->font_size,
			element->border_radius,
			element->text_align_hor,
			element->text_align_ver
		};
		frame->items.add(box);
	}
	
	for (u32 i = 0; i < element->children.count; i++) {
		Element *child = element->children.getCopy(i);
		paintElement(child, new_position, element->scratch_render_box.size, left_clicked, mouse_pos, frame);
	}
}

void ElementLayoutTree::processPaint(GUIFrame *frame) {
	if(!root.visible) return;
	u32 window_width, window_height;
	Platform::getWindowSize(Platform::getCurrentWindow(), window_width, window_height);
	Vec2 window_size = Vec2(window_width, window_height);
	Vec2 mouse_pos = v2iToV2(Input::getMousePosition());
	const bool left_clicked = Input::wasMouseButtonDownOnce(MouseButton::Left);
	paintElement(&root, Vec2(), window_size, left_clicked, mouse_pos, frame);
}
