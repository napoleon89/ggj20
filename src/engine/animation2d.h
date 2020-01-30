#pragma once

#include <functional>

#include <core/std.h>
#include <core/math.h>
#include <core/render_context.h>

enum class Direction : u32 {
	Up,
	DiagUp,
	DiagDown,
	Down,
	Side,
	Count
};

Direction calcDirectionfromVelocity(const Vec2 velocity);

REFLECT_STRUCT
struct SpriteAnimation {
	REFLECT_FIELD
	Vec2 frame_size = Vec2(400, 400);

	REFLECT_FIELD
	u32 frame_count = 0;

	REFLECT_FIELD
	f32 frame_rate = 15;

	Texture2D sheet;

	Vec2 getMaxFramesInSheetDimensions() const;
	Vec2 getFrameUVScale() const;
	Vec2 getFrameUVOffset(u32 frame_index) const;
	Vec2 getSheetSize() const;
};

struct SpriteAnimationGroup {
	SpriteAnimation animations[(u32)Direction::Count];

	SpriteAnimation* getAnimation(const Direction direction);
};

enum class ResetMode {
	None,
	All,
};

struct AnimationMachine {
	enum class AnimationType {
		Single,
		Loop
	};

	SpriteAnimation* current_animation;
	u32 frame_index = 0;
	f32 frame_timer = 0.0f;
	AnimationType type;
	std::function<void()> callback;

	void progress(f32 delta);
	void loopAnimation(SpriteAnimation* new_animation, const ResetMode reset_mode = ResetMode::All);
	void playAnimation(SpriteAnimation* new_animation, std::function<void()> callback = nullptr);
};