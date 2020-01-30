#include <limits>

#include <engine/physics.h>
#include <engine/debug_renderer.h>

Vec2 AABB::getCenter() const {
	return (min + max) * 0.5f;
}

Vec2 AABB::getSize() const {
	return max - min;
}

AABB AABB::inflate(f32 radius) const {
	return { min - radius, max + radius };
}

Vec2 AABB::topRight() const {
	return Vec2(max.x, min.y);
}

Vec2 AABB::topLeft() const {
	return Vec2(min.x, min.y);
}

Vec2 AABB::bottomRight() const {
	return Vec2(max.x, max.y);
}

Vec2 AABB::bottomLeft() const {
	return Vec2(min.x, max.y);
}

Vec2 AABB::closestPoint(Vec2 point) const {
	f32 x = Math::max(min.x, Math::min(point.x, max.x));
	f32 y = Math::max(min.y, Math::min(point.y, max.y));
	return Vec2(x, y);
}

bool AABB::intersects(const AABB& aabb) const {
	return (min.x <= aabb.max.x && max.x >= aabb.min.x) &&
         (min.y <= aabb.max.y && max.y >= aabb.min.y);
}

bool AABB::pointInside(const Vec2 point) const {
	return (min.x <= point.x && max.x >= point.x) &&
         (min.y <= point.y && max.y >= point.y);
}

Vec2 Plane::closestPoint(Vec2 p) const {
	Vec2 project_point = point + Vec2::project(p - point, Vec2::rPerp(normal));
	return project_point;
}

f32 Plane::distance(Vec2 p) const {
	return Vec2::length(p - closestPoint(p));
}

bool Plane::isPointInside(Vec2 p) const {
	return Vec2::dot(normal, Vec2::normalize(p - point)) > 0.0f;
}

bool Plane::isAABBInside(const AABB& aabb) const {
	if(isPointInside(aabb.topLeft())) return true;
	if(isPointInside(aabb.topRight())) return true;
	if(isPointInside(aabb.bottomLeft())) return true;
	if(isPointInside(aabb.bottomRight())) return true;
	if(intersects(aabb)) return true;
	return false;
}

bool Plane::isCircleInside(const Circle& circle) const {
	if(isPointInside(circle.center)) return true;
	if(distance(circle.center) <= circle.radius) return true;
	return false;
}

bool Plane::intersects(const AABB& aabb) const {
	Vec2 c = aabb.getCenter();
	Vec2 e = aabb.max - c;
	f32 r = e.x*Math::abs(normal.x) + e.y*Math::abs(normal.y);
	f32 s = distance(c);
	return Math::abs(s) <= r;
}

Vec2 Line::closestPoint(const Vec2 point) const {
	Vec2 ab = end - start;
	// Project c onto ab, computing parameterized position d(t)=a+ t*(b � a)
	f32 t = Vec2::dot(point - start, ab) / Vec2::dot(ab, ab);
	// If outside segment, clamp t (and therefore d) to the closest endpoint
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	// Compute projected position from the clamped t
	Vec2 d = start + ab * t;		
	return d;
}

Vec2 Circle::closestPoint(Vec2 point) const {
	return center + Vec2::normalize(point - center) * radius;
}

bool Circle::pointInside(Vec2 point) const {
	return Vec2::length(point - center) <= radius;
}

bool Circle::intersects(const Circle& a, const Circle& b) {
	f32 total_radius = a.radius + b.radius;
	f32 distance = Vec2::length(a.center - b.center);
	return distance <= total_radius;
}

namespace CollisionUtils {

	#define EPSILON 0.01f

	bool clipLine(int d, const AABB &box, Vec2 v0, Vec2 v1, f32 &f_low, f32 &f_high) {
		f32 f_dim_low, f_dim_high;
		f_dim_low = (box.min.xy[d] - v0.xy[d]) / (v1.xy[d] - v0.xy[d]);
		f_dim_high = (box.max.xy[d] - v0.xy[d]) / (v1.xy[d] - v0.xy[d]);
		
		if(f_dim_high < f_dim_low) Swap(f_dim_high, f_dim_low);
		if(f_dim_high < f_low) return false;
		if(f_dim_low > f_high) return false;
		f_low = Math::max(f_dim_low, f_low);
		f_high = Math::min(f_dim_high, f_high);
		
		if(f_low > f_high) return false;
		return true;
	}

	bool lineAABBIntersects(Vec2 v0, Vec2 v1, const AABB &box, Vec2 &intersection, f32 &fraction) {
		f32 f_low = 0;
		f32 f_high = 1;
		
		if(!clipLine(0, box, v0, v1, f_low, f_high)) return false;	
		if(!clipLine(1, box, v0, v1, f_low, f_high)) return false;	
		
		Vec2 b = v1 - v0;
		intersection = v0 + b * f_low;
		fraction =  f_low;
		return true;	
	}

	int lineCircleIntersects(Vec2 p, Vec2 p1, Vec2 s, f32 r, f32 &t, Vec2 &q) {
		Vec2 d = Vec2::normalize(p1 - p);
		Vec2 m= p - s;
		f32 b = Vec2::dot(m, d);
		f32 c = Vec2::dot(m, m) - r * r;
		// Exit if r�s origin outside s (c > 0) and r pointing away from s (b > 0)
		if (c > 0.0f && b > 0.0f) return 0;
		f32 discr = b*b - c;
		// A negative discriminant corresponds to ray missing sphere
		if (discr < 0.0f) return 0;
		
		// Ray now found to intersect sphere, compute smallest t value of intersection
		f32 tb = (-b - Math::squareRoot(discr));
		f32 l = Vec2::length(p1 - p);
		t = tb / l;
		// If t is negative, ray started inside sphere so clamp t to zero
		if (t < 0.0f) t = 0.0f;
		q = p + d * tb;
		if(t > 1.0f) return 0;
		return 1;
	}


	bool sweptCircleAABBIntersects(Vec2 circle_start, Vec2 circle_end, f32 radius,  AABB collider, Vec2 &intersection, f32 &fraction) {
		AABB inflated = collider.inflate(radius);
		// renderAABB(renderer, inflated);
		Vec2 test_intersection;
		f32 test_fraction;
		bool inflated_col = lineAABBIntersects(circle_start, circle_end, inflated, test_intersection, test_fraction);
		if(!inflated_col) return false;
		test_fraction = std::numeric_limits<float>::max();
		fraction = 1.0f;
		Vec2 points[4] = { collider.topLeft(), collider.topRight(), collider.bottomLeft(), collider.bottomRight()};
		for(u32 i = 0; i < 4; i++) {
			// renderEllipsoid(renderer, points[i], radius);
			if(lineCircleIntersects(circle_start, circle_end, points[i], radius, test_fraction, test_intersection) && test_fraction < fraction) {
				fraction = test_fraction;
				intersection = test_intersection;
			}
		}
		
		AABB top = {Vec2(collider.min.x, inflated.min.y), Vec2(collider.max.x, collider.min.y) };
		AABB bottom = {Vec2(collider.min.x, collider.max.y), Vec2(collider.max.x, inflated.max.y) };
		AABB right = {Vec2(collider.max.x, collider.min.y), Vec2(inflated.max.x, collider.max.y) };
		AABB left = {Vec2(inflated.min.x, collider.min.y), Vec2(collider.min.x, collider.max.y) };
		
		AABB boundaries[4] = {
			top,bottom,right,left
		};
		
		for(u32 i = 0; i < 4; i++) {
			// renderAABB(renderer, boundaries[i]);
			if(lineAABBIntersects(circle_start, circle_end, boundaries[i], test_intersection, test_fraction) && test_fraction < fraction) {
				fraction = test_fraction;
				intersection = test_intersection;
			}
		}
		if(fraction < 1.0f) return true;
		else return false;
	}

	f32 signed2DTriArea(Vec2 a, Vec2 b, Vec2 c) {
		return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
	}

	bool segmentIntersect(Vec2 a, Vec2 b, Vec2 c, Vec2 d, f32 &t, Vec2 &p) {
		// Sign of areas correspond to which side of ab points c and d are
		f32 a1 = signed2DTriArea(a, b, d); // Compute winding of abd (+ or -)
		f32 a2 = signed2DTriArea(a, b, c); // To intersect, must have sign opposite of a1
		// If c and d are on different sides of ab, areas have different signs
		if (a1 * a2 < 0.0f) {
			// Compute signs for a and b with respect to segment cd
			f32 a3 = signed2DTriArea(c, d, a); // Compute winding of cda (+ or -)
			// Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
			// f32 a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
			f32 a4 = a3 + a2 - a1;
			// Points a and b on different sides of cd if areas have different signs
			if (a3 * a4 < 0.0f) {
				// Segments intersect. Find intersection point along L(t) = a + t * (b - a).
				// Given height h1 of an over cd and height h2 of b over cd,
				// t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
				// where b (the base of the triangles cda and cdb, i.e., the length
				// of cd) cancels out.
				t = a3 / (a3 - a4);
				p = a + t * (b - a);
				return true;
			}
		}
		// Segments not intersecting (or collinear)
		return false;
	}

}

static Vec2 fucked_point = Vec2();

CollisionStats PhysicsWorld::sweptQuery(Vec2 pos, f32 radius, Vec2 vel) const {
	CollisionStats result = {};
	
	Vec2 intersection_point;
	Vec2 test_intersection_point;
	Vec2 actual_intersection;
	
	f32 fraction = std::numeric_limits<float>::max();
	f32 test_fraction;
	bool intersects = false;
	
	for(u32 i = 0; i < static_aabbs.count; i++) {
		const AABB& collider = static_aabbs.getRefConst(i);
		
		// CollisionUtils::lineAABBIntersects(pos, pos+vel, collider.inflate(radius), test_intersection_point, test_fraction)
		if(CollisionUtils::sweptCircleAABBIntersects(pos, pos + vel, radius, collider,  test_intersection_point, test_fraction) && test_fraction < fraction) {
			intersects = true;
			intersection_point = test_intersection_point;
			actual_intersection = collider.closestPoint(intersection_point);
			fraction = test_fraction;

			if(actual_intersection == intersection_point) {
				fucked_point = actual_intersection;
			}
		}
	}

	DebugRenderQueue::addCircle(fucked_point, 0.1f, Colors::fuchsia);
	
	result.hit = intersects;
	if(!intersects) {
		return result;
	}
	
	
	f32 distance = (Vec2::length(vel) * fraction);
	Vec2 touch_point = pos + vel * fraction;
	f32 short_distance = Math::max(distance - EPSILON, 0);
	Vec2 near_point = pos + Vec2::normalize(vel) * short_distance;
	Vec2 surface_normal = Vec2::normalize(intersection_point - actual_intersection);
	result.sliding_plane = {actual_intersection, surface_normal};
	result.fraction = fraction;
	result.near_point = near_point;
	
	return result;
}

Vec2 PhysicsWorld::computeCollideAndSlide(Vec2 pos, f32 radius, Vec2 vel, bool &hit, Vec2 &normal) {
	Vec2 dest = pos + vel;
	Plane first_plane = {};
	hit = false;
	for(int i = 0; i < 3; i++) {
		CollisionStats stats = sweptQuery(pos, radius, vel);		
		if(stats.hit) {
			normal = stats.sliding_plane.normal;
			hit = true;
		}
		
		if(!stats.hit) return dest;
		pos = stats.near_point;
		if(i == 0) {
			f32 long_radius = radius + EPSILON;
			first_plane = stats.sliding_plane;
			dest -= (first_plane.distance(dest) - long_radius) * first_plane.normal;
			vel = dest - pos;
		} else if (i == 1) {
			Plane second_plane = stats.sliding_plane;
			Vec2 crease = Vec2::normalize(first_plane.normal + second_plane.normal);
			f32 dis = Vec2::dot(dest - pos, crease);
			vel = dis * crease;
			dest = pos + vel;
			// Vec2 crease = Vec2::cross()
		}
	}
	
	return pos;	
}