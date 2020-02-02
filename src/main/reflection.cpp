#include <engine/reflection.h>
#include <core/math.h>
#include <string>
#include <cstring>
#include <cstdio>

using namespace Reflection;


template<>
Class *Reflection::getStruct<Vec2>() {
	static Class result;
	result.name = "Vec2";
	result.fields = new Field[2];
	result.field_count = 2;
	result.size = sizeof(Vec2);

	result.fields[0].type = getType<f32>();
	result.fields[0].name = "x";
	result.fields[0].offset = offsetof(Vec2, x);
	result.fields[0].static_address = nullptr;


	result.fields[1].type = getType<f32>();
	result.fields[1].name = "y";
	result.fields[1].offset = offsetof(Vec2, y);
	result.fields[1].static_address = nullptr;


	return &result;
}

template<>
const Type *Reflection::getType<Vec2>() { 
	return (const Type *)getStruct<Vec2>();
}

template<>
Class *Reflection::getStruct<Vec2i>() {
	static Class result;
	result.name = "Vec2i";
	result.fields = new Field[2];
	result.field_count = 2;
	result.size = sizeof(Vec2i);

	result.fields[0].type = getType<s32>();
	result.fields[0].name = "x";
	result.fields[0].offset = offsetof(Vec2i, x);
	result.fields[0].static_address = nullptr;


	result.fields[1].type = getType<s32>();
	result.fields[1].name = "y";
	result.fields[1].offset = offsetof(Vec2i, y);
	result.fields[1].static_address = nullptr;


	return &result;
}

template<>
const Type *Reflection::getType<Vec2i>() { 
	return (const Type *)getStruct<Vec2i>();
}

// == REFLECT_DATA_BEGIN
#include <core/math.h>
#include <engine/animation2d.h>
#include <engine/input.h>
#include <game/camera.h>

template<>
Class *Reflection::getStruct<Vec3>() {
	static Class result;
	result.name = "Vec3";
	result.fields = new Field[3];
	result.field_count = 3;
	result.size = sizeof(Vec3);

	result.fields[0].type = getType<f32>();
	result.fields[0].name = "x";
	result.fields[0].offset = offsetof(Vec3, x);
	result.fields[0].static_address = nullptr;

	result.fields[1].type = getType<f32>();
	result.fields[1].name = "y";
	result.fields[1].offset = offsetof(Vec3, y);
	result.fields[1].static_address = nullptr;

	result.fields[2].type = getType<f32>();
	result.fields[2].name = "z";
	result.fields[2].offset = offsetof(Vec3, z);
	result.fields[2].static_address = nullptr;



	return &result;
}

template<>
const Type *Reflection::getType<Vec3>() { 
	return (const Type *)getStruct<Vec3>();
}
template<>
Class *Reflection::getStruct<Vec4>() {
	static Class result;
	result.name = "Vec4";
	result.fields = new Field[4];
	result.field_count = 4;
	result.size = sizeof(Vec4);

	result.fields[0].type = getType<f32>();
	result.fields[0].name = "x";
	result.fields[0].offset = offsetof(Vec4, x);
	result.fields[0].static_address = nullptr;

	result.fields[1].type = getType<f32>();
	result.fields[1].name = "y";
	result.fields[1].offset = offsetof(Vec4, y);
	result.fields[1].static_address = nullptr;

	result.fields[2].type = getType<f32>();
	result.fields[2].name = "z";
	result.fields[2].offset = offsetof(Vec4, z);
	result.fields[2].static_address = nullptr;

	result.fields[3].type = getType<f32>();
	result.fields[3].name = "w";
	result.fields[3].offset = offsetof(Vec4, w);
	result.fields[3].static_address = nullptr;



	return &result;
}

template<>
const Type *Reflection::getType<Vec4>() { 
	return (const Type *)getStruct<Vec4>();
}
template<>
Class *Reflection::getStruct<SpriteAnimation>() {
	static Class result;
	result.name = "SpriteAnimation";
	result.fields = new Field[3];
	result.field_count = 3;
	result.size = sizeof(SpriteAnimation);

	result.fields[0].type = getType<
		Vec2>();
	result.fields[0].name = "frame_size";
	result.fields[0].offset = offsetof(SpriteAnimation, frame_size);
	result.fields[0].static_address = nullptr;

	result.fields[1].type = getType<
		u32>();
	result.fields[1].name = "frame_count";
	result.fields[1].offset = offsetof(SpriteAnimation, frame_count);
	result.fields[1].static_address = nullptr;

	result.fields[2].type = getType<
		f32>();
	result.fields[2].name = "frame_rate";
	result.fields[2].offset = offsetof(SpriteAnimation, frame_rate);
	result.fields[2].static_address = nullptr;



	return &result;
}

template<>
const Type *Reflection::getType<SpriteAnimation>() { 
	return (const Type *)getStruct<SpriteAnimation>();
}
template<>
Class *Reflection::getStruct<Thing>() {
	static Class result;
	result.name = "Thing";
	result.fields = new Field[1];
	result.field_count = 1;
	result.size = sizeof(Thing);

	result.fields[0].type = getType<
		Vec2>();
	result.fields[0].name = "some_random_thing";
	result.fields[0].offset = offsetof(Thing, some_random_thing);
	result.fields[0].static_address = nullptr;



	return &result;
}

template<>
const Type *Reflection::getType<Thing>() { 
	return (const Type *)getStruct<Thing>();
}
template<>
Class *Reflection::getStruct<Settings>() {
	static Class result;
	result.name = "Settings";
	result.fields = new Field[4];
	result.field_count = 4;
	result.size = sizeof(Settings);

	result.fields[0].type = getType<Vec2>();
	result.fields[0].name = "window_size";
	result.fields[0].static_address = (void *)&Settings::window_size;
	result.fields[0].offset = 0;
	result.fields[1].type = getType<Thing>();
	result.fields[1].name = "ok";
	result.fields[1].static_address = (void *)&Settings::ok;
	result.fields[1].offset = 0;
	result.fields[2].type = getType<bool>();
	result.fields[2].name = "fullscreen";
	result.fields[2].static_address = (void *)&Settings::fullscreen;
	result.fields[2].offset = 0;
	result.fields[3].type = getType<std::string>();
	result.fields[3].name = "window_title";
	result.fields[3].static_address = (void *)&Settings::window_title;
	result.fields[3].offset = 0;


	return &result;
}

template<>
const Type *Reflection::getType<Settings>() { 
	return (const Type *)getStruct<Settings>();
}
