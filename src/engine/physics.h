#pragma once

#include <core/math.h>
#include <core/collections/dynamic_array.h>

struct AABB {
	Vec2 min, max;

	Vec2 getCenter() const;
	Vec2 getSize() const;
	AABB inflate(f32 radius) const;

	Vec2 topRight() const;
	Vec2 topLeft() const;
	Vec2 bottomRight() const;
	Vec2 bottomLeft() const;

	Vec2 closestPoint(Vec2 point) const;

	bool intersects(const AABB& aabb) const;

	bool pointInside(const Vec2 point) const;
};

struct Line {
	Vec2 start, end;

	Vec2 closestPoint(const Vec2 point) const;
};

struct Circle {
	Vec2 center;
	f32 radius;
	
	Vec2 closestPoint(Vec2 point) const;
	bool pointInside(Vec2 point) const;
	static bool intersects(const Circle& a, const Circle& b);
};

struct Plane {
	Vec2 point;
	Vec2 normal;
	
	Vec2 closestPoint(Vec2 p) const;
	f32 distance(Vec2 p) const;
	bool isPointInside(Vec2 p) const;
	bool isAABBInside(const AABB& aabb) const;
	bool isCircleInside(const Circle& circle) const;
	bool intersects(const AABB& aabb) const;
};

namespace CollisionUtils {
	bool clipLine(int d, const AABB &box, Vec2 v0, Vec2 v1, f32 &f_low, f32 &f_high);
	bool lineAABBIntersects(Vec2 v0, Vec2 v1, const AABB &box, Vec2 &intersection, f32 &fraction);
	int lineCircleIntersects(Vec2 p, Vec2 p1, Vec2 s, f32 r, f32 &t, Vec2 &q);
	bool sweptCircleAABBIntersects(Vec2 circle_start, Vec2 circle_end, f32 radius,  AABB collider, Vec2 &intersection, f32 &fraction);
	f32 signed2DTriArea(Vec2 a, Vec2 b, Vec2 c);
	bool segmentIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d, f32 &t, Vec2 &p);
};

struct CollisionStats {
	f32 fraction;
	Plane sliding_plane;	
	Vec2 near_point;
	bool hit;
};

struct PhysicsWorld {
	DynamicArray<AABB> static_aabbs;
	
	CollisionStats sweptQuery(Vec2 pos, f32 radius, Vec2 vel) const;
	Vec2 computeCollideAndSlide(Vec2 pos, f32 radius, Vec2 vel, bool &hit, Vec2 &normal);
};