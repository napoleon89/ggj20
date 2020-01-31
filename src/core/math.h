#pragma once

#include <core/std.h>
#include <cmath>
#include <random>


namespace Math
{
	const f32 Pi32 = 3.14159265359f;
	const f64 Pi = 3.14159265358979323846;

	inline f32 toRadians(f32 degrees) {
		return ((degrees * Math::Pi32) / 180.0f);
	}

	inline f32 toDegrees(f32 radians) {
		return ((radians * 180.0f) / Math::Pi32);
	}

	inline f32 sign(f32 a) {
		return (a > 0) ? 1 : (a < 0) ? -1 : 0;
	}

	inline f32 cos(f32 radians) {
		return std::cos(radians);
	}

	inline f32 sin(f32 radians) {
		return std::sin(radians);
	}

	inline f32 tan(f32 radians) {
		return std::tan(radians);
	}

	inline f32 acos(f32 radians) {
		return std::acos(radians);
	}

	inline f32 cot(f32 radians) {
		return 1.0f / tan(radians);
	}

	inline f32 atan2(f32 x, f32 y) {
		return std::atan2(x, y);
	}

	inline f32 wrap(f32 a, f32 min, f32 max) {
		if(a < min) return max + (min + a);
		else if(a > max) return min + (a - max-1);
		return a;
	}

	inline u32 wrap(u32 a, u32 min, u32 max) {
		if(a < min) return max + (min + a);
		else if(a > max) return min + (a - max-1);
		return a;
	}

	inline s32 wrap(s32 a, s32 min, s32 max) {
		if(a < min) return max + (min + a);
		else if(a > max) return min + (a - max-1);
		return a;
	}

	inline f32 clamp(f32 a, f32 min, f32 max) {
		if(a < min) return min;
		else if(a > max) return max;
		return a;
	}

	inline f32 squareRoot(f32 a) {
		return std::sqrt(a);
	}

	inline f32 q_inv_squareRoot(f32 number) {
		long i;
		f32 x2, y;
		const f32 threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long *)&y; // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1); // what the fuck? 
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y)); // 1st iteration
		//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
	}

	inline s32 ceilToInt(f32 a) {
		return (s32)std::ceil(a);
	}

	inline s32 floorToInt(f32 a) {
		return (s32)std::floor(a);
	}

	inline f32 pow(f32 base, f32 exp) {
		return std::pow(base, exp);
	}

	inline f32 abs(f32 a) {
		return std::fabs(a);
	}

	inline f32 min(f32 a, f32 b) {
		return std::fmin(a, b);
	}

	inline f32 max(f32 a, f32 b) {
		return std::fmax(a, b);
	}

	inline f32 randFloat() {
		f32 r = rand() / (f32)RAND_MAX;
		return r;
	} // NOTE(nathan): between 0 and 1

	inline u32 randRange(u32 min, u32 max) {
		f32 r = randFloat();
		u32 width = max - min;
		u32 value = r * width;
		u32 result = min + value;
		return result;
	}

	inline f32 randRange(f32 min, f32 max) {
		f32 r = randFloat();
		f32 width = max - min;
		f32 value = r * width;
		f32 result = min + value;
		return result;
	}

	inline f32 lerp(f32 s, f32 e, f32 t) {
		return s + (e - s) * t;
	}

	inline f32 blerp(f32 c00, f32 c01, f32 c10, f32 c11, f32 tx, f32 ty) {
		return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
	}

	inline bool inRange(f32 x, f32 min, f32 max) {
		return x >= min && x <= max;
	}

	inline bool isPrime(const u32 x) {
		if(x < 2) return false;
		if(x < 4) return true;
		if((x % 2) == 0) return false;
		for(u32 i = 3; i <= floorToInt(squareRoot(x)); i += 2) {
			if((x % i) == 0) return false;
		}
		return true;
	}

	inline u32 nextPrime(const u32 x) {
		u32 result = x;
		while(!isPrime(result)) {
			result++;
		}
		return result;
	}
};


template<typename type>
struct Vec2DTemplate {
	Vec2DTemplate() : x(0), y(0) { }

	Vec2DTemplate(type x, type y) : x(x), y(y) { }

	Vec2DTemplate(type x) : x(x), y(x) { }

	union {
		struct {
			type x, y;
		};

		struct {
			type u, v;
		};

		struct {
			type w, h;
		};

		type xy[2];
	};

	inline Vec2DTemplate<type> operator-() const {
		return Vec2DTemplate<type>(-x, -y);
	};

	bool operator==(const Vec2DTemplate<type>& a) {
		return x == a.x && y == a.y;
	}

	bool operator!=(const Vec2DTemplate<type>& a) {
		return x != a.x || y != a.y;
	}

	inline Vec2DTemplate<type> operator*(type i) {
		return Vec2DTemplate<type>(x * i, y * i);
	}

	inline Vec2DTemplate<type> operator/(type i) {
		return Vec2DTemplate<type>(x / i, y / i);
	}

	inline void operator+=(const Vec2DTemplate<type>& i) {
		x += i.x;
		y += i.y;
	}

	inline void operator-=(const Vec2DTemplate<type>& i) {
		x -= i.x;
		y -= i.y;
	}

	inline void operator*=(const Vec2DTemplate<type>& i) {
		x *= i.x;
		y *= i.y;
	}

	inline void operator/=(const Vec2DTemplate<type>& i) {
		x /= i.x;
		y /= i.y;
	}

	inline void operator*=(type i) {
		x *= i;
		y *= i;
	}

	inline void operator/=(type i) {
		x /= i;
		y /= i;
	}

	static inline type length(const Vec2DTemplate<type>& x) {
		type result = Math::squareRoot((x.x * x.x) + (x.y * x.y));
		return result;
	}

	static inline type lengthSquared(const Vec2DTemplate<type>& x) {
		type result = (x.x * x.x) + (x.y * x.y);
		return result;
	}

	static inline Vec2DTemplate<type> abs(const Vec2DTemplate<type>& x) {
		return Vec2DTemplate<type>(Math::abs(x.x), Math::abs(x.y));
	}

	static inline Vec2DTemplate<type> sign(const Vec2DTemplate<type>& x) {
		return Vec2DTemplate<type>(Math::sign(x.x), Math::sign(x.y));
	}

	static inline Vec2DTemplate<type> rPerp(const Vec2DTemplate<type>& x) {
		return Vec2DTemplate<type>(x.y, -x.x);
	}

	static inline Vec2DTemplate<type> lPerp(const Vec2DTemplate<type>& x) {
		return Vec2DTemplate<type>(-x.y, x.x);
	}

	static inline Vec2DTemplate<type> normalize(const Vec2DTemplate<type>& x) {
		type len = length(x);
		if(len == 0) return Vec2DTemplate<type>();
		else return Vec2DTemplate<type>(x.x / len, x.y / len);
	}

	static inline type dot(const Vec2DTemplate<type>& a, const Vec2DTemplate<type>& b) {
		return (a.x * b.x) + (a.y * b.y);
	}

	static inline Vec2DTemplate<type> project(Vec2DTemplate<type> a, Vec2DTemplate<type> b) {
		return b * (dot(a, b) / dot(b, b));
	}

	static inline Vec2DTemplate<type> right() {
		return Vec2DTemplate<type>(1, 0);
	}

	static inline Vec2DTemplate<type> up() {
		return Vec2DTemplate<type>(0, 1);
	}

	static inline type angle(Vec2DTemplate<type> a, Vec2DTemplate<type> b) {
		return Math::atan2(a.x, a.y) - Math::atan2(b.x, b.y);
	}

	static inline Vec2DTemplate<type> rotate(Vec2DTemplate<type> a, type radians) {
		// type theta = angle(up(), a);
		// theta += radians;
		// return Vec2DTemplate<type>(Math::sin(theta), Math::cos(theta));
		Vec2DTemplate<type> result = Vec2DTemplate<type>();
		result.x = a.x * Math::cos(radians) - a.y * Math::sin(radians);
		result.y = a.x * Math::sin(radians) + a.y * Math::cos(radians);
		return result;


	}

	static inline Vec2DTemplate<type> lerp(Vec2DTemplate<type> a, Vec2DTemplate<type> b, f32 t) {
		return Vec2DTemplate<type>(Math::lerp(a.x, b.x, t), Math::lerp(a.y, b.y, t));
	}

	inline Vec2DTemplate<type> operator+(const Vec2DTemplate<type>& right) const {
		return Vec2DTemplate<type>(x + right.x, y + right.y);
	}

	inline Vec2DTemplate<type> operator-(const Vec2DTemplate<type>& right) const {
		return Vec2DTemplate<type>(x - right.x, y - right.y);
	}

	inline Vec2DTemplate<type> operator*(const Vec2DTemplate<type>& right) const {
		return Vec2DTemplate<type>(x * right.x, y * right.y);
	}

	inline Vec2DTemplate<type> operator/(const Vec2DTemplate<type>& right) const {
		return Vec2DTemplate<type>(x / right.x, y / right.y);
	}
};


typedef Vec2DTemplate<f32> Vec2;
typedef Vec2DTemplate<s32> Vec2i;

template<typename T>
Vec2DTemplate<T> operator*(const f32 left, const Vec2DTemplate<T>& right) {
	return right * left;
}

Vec2 v2iToV2(Vec2i input);

REFLECT_STRUCT

struct Vec3 {
	Vec3(f32 x, f32 y, f32 z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3() {
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	Vec3(Vec2 xy, f32 z) {
		this->x = xy.x;
		this->y = xy.y;
		this->z = z;
	}

	Vec3(f32 x) {
		this->x = x;
		this->y = x;
		this->z = x;
	}

	union {
		struct {
			REFLECT_FIELD f32 x;
			REFLECT_FIELD f32 y;
			REFLECT_FIELD f32 z;
		};

		struct {
			f32 u, v, w;
		};

		struct {
			f32 r, g, b;
		};

		f32 xyz[3];
	};

	Vec3 operator-() {
		return Vec3(-x, -y, -z);
	};

	bool operator==(Vec3 a) {
		return x == a.x && y == a.y && z == a.z;
	}

	Vec3 operator+(const Vec3& i) {
		return Vec3(x + i.x, y + i.y, z + i.z);
	}

	Vec3 operator-(const Vec3& i) {
		return Vec3(x - i.x, y - i.y, z - i.z);
	}

	Vec3 operator*(const Vec3& i) {
		return Vec3(x * i.x, y * i.y, z * i.z);
	}

	Vec3 operator/(const Vec3& i) {
		return Vec3(x / i.x, y / i.y, z / i.z);
	}

	Vec3 operator*(f32 i) {
		return Vec3(x * i, y * i, z * i);
	}

	Vec3 operator/(f32 i) {
		return Vec3(x / i, y / i, z / i);
	}

	void operator+=(const Vec3& i) {
		x += i.x;
		y += i.y;
		z += i.z;
	}

	void operator-=(const Vec3& i) {
		x -= i.x;
		y -= i.y;
		z -= i.z;
	}

	void operator*=(const Vec3& i) {
		x *= i.x;
		y *= i.y;
		z *= i.z;
	}

	void operator/=(const Vec3& i) {
		x /= i.x;
		y /= i.y;
		z /= i.z;
	}

	void operator*=(f32 i) {
		x *= i;
		y *= i;
		z *= i;
	}

	void operator/=(f32 i) {
		x /= i;
		y /= i;
		z /= i;
	}

	static inline f32 dot(const Vec3& a, const Vec3& b) {
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
	}

	static inline Vec3 cross(const Vec3& a, const Vec3& b) {
		return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	static inline Vec3 normalize(Vec3 a) {
		return a / Vec3::length(a);
	}

	static inline f32 lengthSquared(const Vec3& x) {
		f32 result = (x.x * x.x) + (x.y * x.y) + (x.z * x.z);
		return result;
	}

	static inline f32 length(const Vec3& x) {
		f32 result = Math::squareRoot(lengthSquared(x));
		return result;
	}

	static inline Vec3 project(Vec3 a, Vec3 b) {
		return b * (dot(a, b) / dot(b, b));
	}

	static inline f32 distanceToPlane(Vec3 point, Vec3 plane_origin, Vec3 plane_normal) {
		return dot(plane_normal, (point - plane_origin));
	}

	static inline Vec3 lerp(Vec3 v0, Vec3 v1, f32 t) {
		Vec3 result = Vec3();
		result.x = Math::lerp(v0.x, v1.x, t);
		result.y = Math::lerp(v0.y, v1.y, t);
		result.z = Math::lerp(v0.z, v1.z, t);
		return result;
	}

	static inline Vec3 abs(Vec3 a) {
		return Vec3(Math::abs(a.x), Math::abs(a.y), Math::abs(a.z));
	}

	static inline int maxIndex(Vec3 a) {
		f32 val = 0;
		int index = -1;
		for(int i = 0; i < 3; i++) {
			if(a.xyz[i] > val) {
				index = i;
				val = a.xyz[i];
			}
		}
		return index;
	}

	static inline Vec3 rmax(Vec3 a, Vec3 b) {
		Vec3 result = Vec3();
		result.x = Math::max(a.x, b.x);
		result.y = Math::max(a.y, b.y);
		result.z = Math::max(a.z, b.z);
		return result;
	}

	static inline Vec3 rmin(Vec3 a, Vec3 b) {
		Vec3 result = Vec3();
		result.x = Math::min(a.x, b.x);
		result.y = Math::min(a.y, b.y);
		result.z = Math::min(a.z, b.z);
		return result;
	}

	static Vec3 up;
	static Vec3 forward;
	static Vec3 right;
};

inline Vec3 operator*(f32 i, Vec3 a) {
	return a * i;
}


inline Vec3 operator/(f32 i, Vec3 a) {
	return a / i;
}

REFLECT_STRUCT

struct Vec4 {
	Vec4(f32 x, f32 y, f32 z, f32 w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vec4() {
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vec4(Vec2 xy, Vec2 zw) {
		this->x = xy.x;
		this->y = xy.y;
		this->z = zw.x;
		this->w = zw.y;
	}

	Vec4(Vec2 xy, f32 z, f32 w) {
		this->x = xy.x;
		this->y = xy.y;
		this->z = z;
		this->w = w;
	}

	Vec4(Vec3 xyz, f32 w) {
		this->x = xyz.x;
		this->y = xyz.y;
		this->z = xyz.z;
		this->w = w;
	}

	Vec4(f32 x) {
		this->x = x;
		this->y = x;
		this->z = x;
		this->w = x;
	}

	union {
		struct {
			REFLECT_FIELD f32 x;
			REFLECT_FIELD f32 y;
			REFLECT_FIELD f32 z;
			REFLECT_FIELD f32 w;
		};

		struct {
			f32 r, g, b, a;
		};

		f32 xyzw[4];
	};

	Vec4 operator-() {
		return Vec4(-x, -y, -z, -w);
	};

	Vec4 operator+(const Vec4& i) {
		return Vec4(x + i.x, y + i.y, z + i.z, w + i.w);
	}

	Vec4 operator-(const Vec4& i) {
		return Vec4(x - i.x, y - i.y, z - i.z, w - i.w);
	}

	Vec4 operator*(const Vec4& i) {
		return Vec4(x * i.x, y * i.y, z * i.z, w * i.w);
	}

	Vec4 operator/(const Vec4& i) {
		return Vec4(x / i.x, y / i.y, z / i.z, w / i.w);
	}

	Vec4 operator*(f32 i) {
		return Vec4(x * i, y * i, z * i, w * i);
	}

	Vec4 operator/(f32 i) {
		return Vec4(x / i, y / i, z / i, w / i);
	}

	void operator+=(const Vec4& i) {
		x += i.x;
		y += i.y;
		z += i.z;
		w += i.z;
	}

	void operator-=(const Vec4& i) {
		x -= i.x;
		y -= i.y;
		z -= i.z;
		w -= i.z;
	}

	void operator*=(const Vec4& i) {
		x *= i.x;
		y *= i.y;
		z *= i.z;
		w *= i.z;
	}

	void operator/=(const Vec4& i) {
		x /= i.x;
		y /= i.y;
		z /= i.z;
		w /= i.z;
	}

	void operator*=(f32 i) {
		x *= i;
		y *= i;
		z *= i;
		w *= i;
	}

	void operator/=(f32 i) {
		x /= i;
		y /= i;
		z /= i;
		w /= i;
	}

	static inline f32 length(const Vec4& x) {
		f32 result = Math::squareRoot((x.x * x.x) + (x.y * x.y) + (x.z * x.z) + (x.w * x.w));
		return result;
	}

	static inline f32 dot(const Vec4& a, const Vec4& b) {
		return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
	}

	static inline Vec4 normalize(Vec4 a) {
		return a / Vec4::length(a);
	}
};

struct Color : public Vec4 {
	Color() : Vec4() {}
	Color(f32 r, f32 g, f32 b, f32 a) : Vec4(r, g, b, a) {}
	Color(f32 r, f32 g, f32 b) : Vec4(r, g, b, 1.0f) { }

	Color(const Vec4 v) : Vec4(v) {}

	inline u32 toU32() const {
		u32 ur = (u32)(r * 255.0f);
		u32 ug = (u32)(g * 255.0f);
		u32 ub = (u32)(b * 255.0f);
		u32 ua = (u32)(a * 255.0f);
		u32 result = RGBA(ur, ug, ub, ua);
		return result;
	}
};

namespace Colors {
	inline static Color white = Color(1.0f, 1.0f, 1.0f);
	inline static Color silver = Color(0.75f, 0.75f, 0.75f);
	inline static Color gray = Color(0.5f, 0.5f, 0.5f);
	inline static Color black = Color(0, 0, 0);
	inline static Color red = Color(1.0f, 0, 0);
	inline static Color maroon = Color(0.5f, 0, 0);
	inline static Color yellow = Color(1.0f, 1.0f, 0);
	inline static Color olive = Color(0.5f, 0.5f, 0);
	inline static Color lime = Color(0, 1.0f, 0);
	inline static Color green = Color(0, 0.5f, 0);
	inline static Color aqua = Color(0, 1.0f, 1.0f);
	inline static Color teal = Color(0, 0.5f, 0.5f);
	inline static Color blue = Color(0, 0, 1.0f);
	inline static Color navy = Color(0, 0, 0.5f);
	inline static Color fuchsia = Color(1.0f, 0, 1.0f);
	inline static Color purple = Color(0.5f, 0, 0.5f);
	inline static Color transparent = Color(0.0f, 0.0f, 0.0f, 0.0f);
}

struct Mat4 {
	union {
		f32 data2d[4][4];
		f32 data1d[16];
	};

	Mat4() {
		data2d[0][0] = 1.0f;
		data2d[0][1] = 0.0f;
		data2d[0][2] = 0.0f;
		data2d[0][3] = 0.0f;
		data2d[1][0] = 0.0f;
		data2d[1][1] = 1.0f;
		data2d[1][2] = 0.0f;
		data2d[1][3] = 0.0f;
		data2d[2][0] = 0.0f;
		data2d[2][1] = 0.0f;
		data2d[2][2] = 1.0f;
		data2d[2][3] = 0.0f;
		data2d[3][0] = 0.0f;
		data2d[3][1] = 0.0f;
		data2d[3][2] = 0.0f;
		data2d[3][3] = 1.0f;
	}

	Mat4 operator*(const Mat4& other) {
		Mat4 result = Mat4();
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				result.data2d[i][j] = data2d[i][0] * other.data2d[0][j] +
						data2d[i][1] * other.data2d[1][j] +
						data2d[i][2] * other.data2d[2][j] +
						data2d[i][3] * other.data2d[3][j];
			}
		}
		return result;
	}

	void operator*=(const Mat4& other) {
		Mat4 m = *this * other;
		*this = m;
	}


	/*
	
	Vector result;
	for ( int i = 0; i < 4; ++i )
	   result[i] = v[0] * m[0][i] + v[1] * m[1][i] + v[2] + m[2][i] + v[3] * m[3][i];
	result[0] = result[0]/result[3];
	result[1] = result[1]/result[3];
	result[2] = result[2]/result[3];
	return result;
	
	*/

	static inline Vec4 transform(const Mat4& m, const Vec4& other) {
		Vec4 result = Vec4();
		for(int i = 0; i < 4; i++) {
			f32 r = 0.0f;
			for(int j = 0; j < 4; j++) {
				r += other.xyzw[j] * m.data2d[i][j];
			}
			result.xyzw[i] = r;
		}

		// result.x /= result.w;
		// result.y /= result.w;
		// result.z /= result.w;

		return result;
	}

	static Mat4 ortho(f32 width, f32 height, f32 near_plane, f32 far_plane) {
		Mat4 result = Mat4();
		result.data2d[0][0] = 2.0f / width;
		result.data2d[1][1] = 2.0f / height;
		result.data2d[2][2] = 2.0f / (far_plane - near_plane);
		// result.data2d[2][3] = ((far_plane + near_plane) / (far_plane - near_plane));
		return result;
	}

	static inline Mat4 perspective(f32 fov, f32 aspect_ratio, f32 z_near, f32 z_far) {

		Mat4 result = Mat4();

		f32 tan_half_fov = Math::tan(Math::toRadians(fov * 0.5f));

		// scaling

		result.data2d[0][0] = (1.0f / tan_half_fov) / aspect_ratio;
		result.data2d[1][1] = 1.0f / tan_half_fov;
		result.data2d[2][2] = z_far / (z_far - z_near);

		// translation

		result.data2d[3][2] = 1.0f;

		// z part
		result.data2d[2][3] = -z_near * z_far / (z_far - z_near);


		result.data2d[3][3] = 0.0f;
		return result;
	}

	static inline Mat4 invPerspective(const Mat4& perspective) {
		Mat4 result = Mat4();

		f32 d = perspective.data2d[2][3];
		f32 e = perspective.data2d[3][2];
		f32 c = perspective.data2d[2][2];

		result.data2d[0][0] = 1.0f / perspective.data2d[0][0];
		result.data2d[1][1] = 1.0f / perspective.data2d[1][1];
		result.data2d[2][2] = 0.0f;

		result.data2d[2][3] = 1.0f / d;
		result.data2d[3][3] = -(c / (d * e));
		result.data2d[3][2] = 1.0f / e;
		return result;
		// return invMat(perspective);
	}

	static inline Mat4 translate(Vec3 a) {
		Mat4 result = Mat4();
		result.data2d[0][3] = a.x;
		result.data2d[1][3] = a.y;
		result.data2d[2][3] = a.z;
		return result;
	}

	static inline Mat4 rotateZ(f32 rads) {
		Mat4 result = Mat4();
		result.data2d[0][0] = Math::cos(rads);
		result.data2d[1][1] = Math::cos(rads);
		result.data2d[0][1] = -Math::sin(rads);
		result.data2d[1][0] = Math::sin(rads);
		return result;
	}

	static inline Mat4 rotateY(f32 rads) {
		Mat4 result = Mat4();
		result.data2d[0][0] = Math::cos(rads);
		result.data2d[0][2] = Math::sin(rads);
		result.data2d[2][0] = -Math::sin(rads);
		result.data2d[2][2] = Math::cos(rads);
		return result;
	}

	static inline Mat4 rotateX(f32 rads) {
		Mat4 result = Mat4();
		result.data2d[1][1] = Math::cos(rads);
		result.data2d[1][2] = -Math::sin(rads);
		result.data2d[2][1] = Math::sin(rads);
		result.data2d[2][2] = Math::cos(rads);
		return result;
	}


	static inline Mat4 scale(Vec3 a) {
		Mat4 result = Mat4();
		result.data2d[0][0] = a.x;
		result.data2d[1][1] = a.y;
		result.data2d[2][2] = a.z;
		return result;
	}

	static inline Mat4 inverseScale(const Mat4& a) {
		Mat4 result = Mat4();
		result.data2d[0][0] = 1.0f / a.data2d[0][0];
		result.data2d[1][1] = 1.0f / a.data2d[1][1];
		result.data2d[2][2] = 1.0f / a.data2d[2][2];
		return result;
	}

	static inline Mat4 inverseRotation(const Mat4& a) {
		Mat4 result = a;
		result.data2d[0][1] *= -1.0f;
		result.data2d[1][0] *= -1.0f;
		return result;
	}

	static inline Mat4 inverseTranslation(const Mat4& a) {
		Mat4 result = a;
		result.data2d[0][3] *= -1.0f;
		result.data2d[1][3] *= -1.0f;
		result.data2d[2][3] *= -1.0f;
		return result;
	}

	static inline Mat4 lookAt(Vec3 position, Vec3 direction, Vec3 up) {
		Mat4 result = Mat4();
		Vec3 cam_side = Vec3::normalize(Vec3::cross(-direction, up));
		Vec3 cam_up = Vec3::cross(cam_side, -direction);

		result.data2d[0][0] = cam_side.x;
		result.data2d[0][1] = cam_side.y;
		result.data2d[0][2] = cam_side.z;
		result.data2d[1][0] = cam_up.x;
		result.data2d[1][1] = cam_up.y;
		result.data2d[1][2] = cam_up.z;
		result.data2d[2][0] = direction.x;
		result.data2d[2][1] = direction.y;
		result.data2d[2][2] = direction.z;

		result = result * Mat4::translate(position);

		return result;
	}

	static inline Mat4 basisChange(Vec3 right, Vec3 up, Vec3 forward) {
		Mat4 result = {};
		result.data2d[0][0] = right.x;
		result.data2d[1][0] = right.y;
		result.data2d[2][0] = right.z;
		result.data2d[0][1] = up.x;
		result.data2d[1][1] = up.y;
		result.data2d[2][1] = up.z;
		result.data2d[0][2] = forward.x;
		result.data2d[1][2] = forward.y;
		result.data2d[2][2] = forward.z;
		return result;
	}

	static inline Mat4 transpose(const Mat4& a) {
		Mat4 result = Mat4();

		result.data2d[0][0] = a.data2d[0][0];
		result.data2d[1][0] = a.data2d[0][1];
		result.data2d[2][0] = a.data2d[0][2];
		result.data2d[3][0] = a.data2d[0][3];
		result.data2d[0][1] = a.data2d[1][0];
		result.data2d[1][1] = a.data2d[1][1];
		result.data2d[2][1] = a.data2d[1][2];
		result.data2d[3][1] = a.data2d[1][3];
		result.data2d[0][2] = a.data2d[2][0];
		result.data2d[1][2] = a.data2d[2][1];
		result.data2d[2][2] = a.data2d[2][2];
		result.data2d[3][2] = a.data2d[2][3];
		result.data2d[0][3] = a.data2d[3][0];
		result.data2d[1][3] = a.data2d[3][1];
		result.data2d[2][3] = a.data2d[3][2];
		result.data2d[3][3] = a.data2d[3][3];

		return result;
	}

	static inline Vec3 extractTranslation(const Mat4& a) {
		Vec3 result = Vec3();

		result.x = a.data2d[0][3];
		result.y = a.data2d[1][3];
		result.z = a.data2d[2][3];
		return result;
	}

	static inline Vec3 extractScale(const Mat4& a) {
		Vec3 result = Vec3();
		result.x = a.data2d[0][0];
		result.y = a.data2d[1][1];
		result.z = a.data2d[2][2];
		return result;
	}
};

inline Mat4 operator*(const Mat4& left, const Mat4& right) {
	Mat4 result = Mat4();
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			result.data2d[i][j] = left.data2d[i][0] * right.data2d[0][j] +
					left.data2d[i][1] * right.data2d[1][j] +
					left.data2d[i][2] * right.data2d[2][j] +
					left.data2d[i][3] * right.data2d[3][j];
		}
	}
	return result;
}

struct Quat {
	union {
		struct {
			f32 x, y, z, w;
		};

		Vec4 vector;
		f32 data[4];
	};

	Quat() {
		x = y = z = 0;
		w = 1;
	}

	Quat(f32 x, f32 y, f32 z, f32 w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Quat(Vec3 axis, f32 angle) {
		Vec3 rv;

		f32 axis_norm = 0;
		f32 sor = 0;

		axis_norm = Math::squareRoot(Vec3::dot(axis, axis));
		sor = Math::sin(angle / 2.0f);
		rv = axis * sor;

		this->w = Math::cos(angle * 0.5f);
		this->vector = Vec4(rv / axis_norm, vector.w);
	}

	Quat operator-() {
		return Quat(-this->x, -this->y, -this->z, -this->w);
	}

	Quat operator/(f32 right) {
		Quat result;
		for(int i = 0; i < 4; i++) {
			result.data[i] = this->data[i] / right;
		}
		return result;
	}

	static inline Quat nlerp(Quat left, Quat right, f32 t) {

		return Quat::normalize(lerp(left, right, t));
	}

	static inline Quat lerp(Quat left, Quat right, f32 t) {
		Quat result;
		for(int i = 0; i < 4; i++) {
			result.data[i] = Math::lerp(left.data[i], right.data[i], t);
		}
		return result;
	}

	static inline Quat interpolate(Quat left, Quat right, f32 t) {
		Quat result = Quat();
		f32 dot = Quat::dot(left, right);
		f32 blend = 1.0f - t;
		if(dot < 0.0f) {
			for(int i = 0; i < 4; i++) {
				result.data[i] = blend * left.data[i] + t * -right.data[i];
			}
		} else {
			for(int i = 0; i < 4; i++) {
				result.data[i] = blend * left.data[i] + t * right.data[i];
			}
		}
		return Quat::normalize(result);
	}

	static inline f32 dot(Quat left, Quat right) {
		return Vec4::dot(left.vector, right.vector);
	}

	static inline Quat normalize(Quat x) {
		f32 length = Math::squareRoot(Quat::dot(x, x));
		return x / length;
	}

	static inline Mat4 toMat4(Quat q) {
		Mat4 result = Mat4();

		Quat nq = normalize(q);

		float xx, yy, zz,
		      XY, XZ, YZ,
		      WX, WY, WZ;

		xx = nq.x * nq.x;
		yy = nq.y * nq.y;
		zz = nq.z * nq.z;
		XY = nq.x * nq.y;
		XZ = nq.x * nq.z;
		YZ = nq.y * nq.z;
		WX = nq.w * nq.x;
		WY = nq.w * nq.y;
		WZ = nq.w * nq.z;

		result.data2d[0][0] = 1.0f - 2.0f * (yy + zz);
		result.data2d[0][1] = 2.0f * (XY + WZ);
		result.data2d[0][2] = 2.0f * (XZ - WY);

		result.data2d[1][0] = 2.0f * (XY - WZ);
		result.data2d[1][1] = 1.0f - 2.0f * (xx + zz);
		result.data2d[1][2] = 2.0f * (YZ + WX);

		result.data2d[2][0] = 2.0f * (XZ + WY);
		result.data2d[2][1] = 2.0f * (YZ - WX);
		result.data2d[2][2] = 1.0f - 2.0f * (xx + yy);

		return Mat4::transpose(result);
	}
};

namespace Math
{
	inline Vec2 lerp(Vec2 a, Vec2 b, f32 t) {
		return a + (b - a) * t;
	}

	inline Vec2 quadCurve(Vec2 a, Vec2 b, Vec2 c, f32 t) {
		Vec2 p0 = lerp(a, b, t);
		Vec2 p1 = lerp(b, c, t);
		return lerp(p0, p1, t);
	}

	inline Vec2 cubCurve(Vec2 a, Vec2 b, Vec2 c, Vec2 d, f32 t) {
		Vec2 p0 = quadCurve(a, b, c, t);
		Vec2 p1 = quadCurve(b, c, d, t);
		return lerp(p0, p1, t);
	}
}
