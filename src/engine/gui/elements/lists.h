#pragma once

#include <engine/gui/gui.h>

struct ListElement : public Element {
	enum class Orientation {
		Horizontal,
		Vertical
	};

	Orientation orientation = Orientation::Horizontal;
};

struct FixedListElement : public ListElement {
	virtual void layoutChildren() override;
};

struct FlexibleElement : public Element {
	f32 flex_factor = 1.0f;
};

struct FlexibleListElement : public ListElement {
	virtual void layoutChildren() override;
};