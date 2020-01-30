#include <engine/animation2d.h>
#include <core/logger.h>

Direction calcDirectionfromVelocity(const Vec2 velocity) {
	Direction result = Direction::Down;
	f32 angle = Math::toDegrees(Math::abs(Vec2::angle(Vec2::up(), velocity)));
	if(Math::inRange(angle, 0.0f, 22.5f)) result = Direction::Up;
	else if(Math::inRange(angle, 22.5f, 67.5f)) result = Direction::DiagUp;
	else if(Math::inRange(angle, 67.5f, 112.5f)) result = Direction::Side;
	else if(Math::inRange(angle, 112.5f, 157.5f)) result = Direction::DiagDown;
	else if(Math::inRange(angle, 157.5f, 180.0f)) result = Direction::Down;
	return result;
}

Vec2 SpriteAnimation::getMaxFramesInSheetDimensions() const {
	Vec2 result = getSheetSize() / frame_size;
	result.x = Math::floorToInt(result.x);
	result.y = Math::floorToInt(result.y);
	return result;
}

Vec2 SpriteAnimation::getFrameUVScale() const {
	return Vec2(1.0f) / getMaxFramesInSheetDimensions();
}

Vec2 SpriteAnimation::getFrameUVOffset(u32 frame_index) const {
	Vec2 frame_count = getMaxFramesInSheetDimensions();
	Vec2 scale = getFrameUVScale();
	f32 x = frame_index % (u32)frame_count.x;
	f32 y = frame_index / (u32)frame_count.x;
	return Vec2(x, y) * scale;
}

Vec2 SpriteAnimation::getSheetSize() const {
	return Vec2(sheet.width, sheet.height);
}

SpriteAnimation* SpriteAnimationGroup::getAnimation(const Direction direction) {
	return &animations[(u32)direction];
}

void AnimationMachine::progress(f32 delta) {	
	if(current_animation == nullptr) return;
	frame_timer += delta;
	if(frame_timer >= (1.0f / current_animation->frame_rate)) {
		frame_timer = 0.0f;
		frame_index++;
		if(frame_index >= current_animation->frame_count) {
			if(type == AnimationType::Single) {
				frame_index = current_animation->frame_count-1;
			} else if(type == AnimationType::Loop) {
				frame_index = 0;
			}
			if(callback != nullptr) {
				callback();
			}
		}
	}
}

void AnimationMachine::loopAnimation(SpriteAnimation* new_animation, const ResetMode reset_mode) {
	if(new_animation == current_animation) return;
	current_animation = new_animation;
	if(reset_mode == ResetMode::All) {
		frame_index = 0;
		frame_timer = 0.0f;
	}
	type = AnimationType::Loop;
	callback = nullptr;
}

void AnimationMachine::playAnimation(SpriteAnimation* new_animation, std::function<void()> callback) {
	frame_index = 0;
	frame_timer = 0.0f;
	current_animation = new_animation;
	type = AnimationType::Single;
	this->callback = callback;
}