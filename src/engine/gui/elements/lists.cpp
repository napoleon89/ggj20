#include <engine/gui/elements/lists.h>

void FixedListElement::layoutChildren() {
	Constraint constraint = {};
	u32 axis = (u32)orientation;
	u32 child_count = children.count;

	constraint.max.xy[axis] = scratch_render_box.size.xy[axis] / (f32)child_count;
	constraint.max.xy[1 - axis] = scratch_render_box.size.xy[1 - axis];


	Vec2 offset = Vec2();
	for (u32 i = 0; i < child_count; i++) {
		Element *child = children.getCopy(i);

		Vec2 child_size = layoutElementSizes(child, constraint);
		child->scratch_render_box.offset = offset;

		
		offset.xy[axis] += constraint.max.xy[axis];
	}
}

void FlexibleListElement::layoutChildren() {
	
	u32 axis = (u32)orientation;
	u32 child_count = children.count;

	f32 total_flex_factor = 0.0f;
	f32 total_non_flex_size = scratch_render_box.size.xy[axis];


	for (u32 i = 0; i < child_count; i++) {
		Element *child = children.getCopy(i);
		FlexibleElement *flex = dynamic_cast<FlexibleElement *>(child);

		Constraint constraint = {Vec2(), scratch_render_box.size};
		
		if (flex) {
			total_flex_factor += flex->flex_factor;
		} else {
			Vec2 child_size = layoutElementSizes(child, constraint);
			total_non_flex_size -= child_size.xy[axis];
		}
	}

	f32 flex_unit_size = total_non_flex_size / total_flex_factor;

	Vec2 offset = Vec2();
	for (u32 i = 0; i < child_count; i++) {
		Element *child = children.getCopy(i);
		FlexibleElement *flex = dynamic_cast<FlexibleElement *>(child);
		
		if (flex) {
			Constraint constraint = {};
			constraint.max.xy[axis] = flex_unit_size * flex->flex_factor;
			constraint.max.xy[1 - axis] = scratch_render_box.size.xy[1 - axis];
			layoutElementSizes(child, constraint);
		}
		
		child->scratch_render_box.offset = offset;
		offset.xy[axis] += child->scratch_render_box.size.xy[axis];
	}
}
