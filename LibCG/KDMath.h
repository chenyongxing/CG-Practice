#ifndef KD_MATH_H
#define KD_MATH_H

/*
	Left-handed coordinate system
	Colume-major matrix	[lie zhu xu]
*/

#include <cstdint>
#include <cfloat>
#include <cmath>
#include <iostream>

typedef uint32_t u32;
typedef int32_t i32;
typedef float f32;
typedef double f64;

#define F32_MAX FLT_MAX
#define F32_INF INFINITY
#define F32_EPS FLT_EPSILON
#define F32_PI 3.14159265358979323846f
#define F32_2PI 6.2831853071795862f
#define F32_1_FRAC_PI 0.318309886183790671538f

#define F64_MAX DBL_MAX
#define F64_INF INFINITY
#define F64_EPS DBL_EPSILON
#define F64_PI 3.14159265358979323846
#define F64_2PI 6.2831853071795862
#define F64_1_FRAC_PI 0.318309886183790671538

template <typename T>
T sign(T a)
{
	return (a < 0) ? (T)-1 : (T)1;
}

template <typename T>
bool equal(T a, T b)
{
	return abs(a - b) < 0.0001;
}

template <typename T>
T min(T a, T b)
{
	return (a < b) ? a : b;
}

template <typename T>
T max(T a, T b)
{
	return (a > b) ? a : b;
}

inline f32 radians(f32 a)
{
	return a * (F32_PI / 180.0f);
}

inline f64 radians(f64 a)
{
	return a * (F64_PI / 180.0);
}

inline f32 degrees(f32 a)
{
	return a * (180.0f / F32_PI);
}

inline f64 degrees(f64 a)
{
	return a * (180.0 / F64_PI);
}

// vec2 Declarations

template <typename T>
struct vec2
{
	T x, y;

	vec2() : x(0), y(0) {};
	vec2(T x) : x(x), y(x) {};
	vec2(T x, T y) : x(x), y(y) {};

	T& operator[](int i)
	{
		if (i == 0) return x;
		return y;
	}

	vec2<T> operator+(vec2<T> v) const
	{
		return vec2<T>(x + v.x, y + v.y);
	}

	vec2<T> operator-(vec2<T> v) const
	{
		return vec2<T>(x - v.x, y - v.y);
	}

	vec2<T> operator*(vec2<T> v) const
	{
		return vec2<T>(x * v.x, y * v.y);
	}

	template <typename U>
	vec2<T> operator*(U s) const
	{
		return vec2<T>(x * s, y * s);
	}

	template <typename U>
	vec2<T> operator/(U f) const
	{
		float inv = (float)(1.0f / f);
		return vec2<T>(x * inv, y * inv);
	}

	inline vec2<T> perpendicular()
	{
		return vec2<T>(y, -x);
	}
};

template <typename T>
bool operator==(const vec2<T>& v1, const vec2<T>& v2)
{
	return v1.x == v2.x && v1.y == v2.y;
}

template <typename T>
bool operator!=(const vec2<T>& v1, const vec2<T>& v2)
{
	return v1.x != v2.x || v1.y != v2.y;
}

template <typename T>
inline T length(const vec2<T>& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

template <typename T>
inline vec2<T> normalize(const vec2<T>& v)
{
	return v / length(v);
}

template <typename T>
inline T dot(const vec2<T>& v1, const vec2<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template <typename T>
inline T cross(const vec2<T>& v1, const vec2<T>& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const vec2<T>& v)
{
	os << "[ " << v.x << ", " << v.y << " ]";
	return os;
}

// vec3 Declarations

template <typename T>
struct vec3
{
	T x, y, z;

	vec3() : x(0), y(0), z(0) {};
	vec3(T x) : x(x), y(x), z(x) {};
	vec3(T x, T y, T z) : x(x), y(y), z(z) {};
	vec3(vec2<T> v2) : x(v2.x), y(v2.y), z(0) {};

	T& operator[](int i) 
	{
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	vec3<T> operator+(const vec3<T>& v) const
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3<T> operator-(const vec3<T>& v) const
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	vec3<T> operator*(vec3<T> v) const
	{
		return vec3<T>(x * v.x, y * v.y, z * v.z);
	}

	vec3<T> operator/(const vec3<T>& v) const
	{
		return vec3(x / v.x, y / v.y, z / v.z);
	}

	template <typename U>
	vec3<T> operator*(U s) const
	{
		return vec3<T>(s * x, s * y, s * z);
	}

	template <typename U>
	vec3<T> operator/(U f) const
	{
		float inv = (float)(1.0f / f);
		return vec3<T>(x * inv, y * inv, z * inv);
	}

	void operator+=(const vec3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator-=(const vec3<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void operator*=(const vec3<T>& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	void operator/=(const vec3<T>& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	template <typename U>
	void operator*=(U s)
	{
		x *= s;
		y *= s;
		z *= s;
	}

	template <typename U>
	void operator/=(U s)
	{
		x /= s;
		y /= s;
		z /= s;
	}
};

template <typename T, typename U>
vec3<T> operator*(U s, const vec3<T>& v)
{
	return vec3<T>(s * v.x, s * v.y, s * v.z);
}

template <typename T>
inline T length(const vec3<T>& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template <typename T>
inline T distance(const vec3<T>& v1, const vec3<T>& v2)
{
	return length(v1 - v2);
}

template <typename T>
inline vec3<T> normalize(const vec3<T>& v) 
{
	return v / length(v);
}

template <typename T>
inline T dot(const vec3<T>& v1, const vec3<T>& v2) 
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
{
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return vec3<T>((v1y * v2z) - (v1z * v2y), (v1z * v2x) - (v1x * v2z), (v1x * v2y) - (v1y * v2x));
}

template <typename T>
vec3<T> min(const vec3<T>& p1, const vec3<T>& p2)
{
	return vec3<T>(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
}

template <typename T>
vec3<T> max(const vec3<T>& p1, const vec3<T>& p2)
{
	return vec3<T>(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
}

template <typename T>
vec3<T> sign(const vec3<T>& v)
{
	return vec3<T>(sign(v.x), sign(v.y), sign(v.z));
}

template <typename T>
vec3<T> equal(const vec3<T>& a, const vec3<T>& b)
{
	return vec3<T>(equal(a.x, b.x), equal(a.y, b.y), equal(a.z, b.z));
}

template <typename T>
vec3<T> faceforward(const vec3<T>& i, const vec3<T>& n)
{
	return dot(i, n) > 0 ? n : n * -1;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const vec3<T>& v) 
{
	os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return os;
}

// vec4 Declarations

template <typename T>
struct vec4
{
	T x, y, z, w;

	vec4() : x(0), y(0), z(0), w(0) {};
	vec4(T x) : x(x), y(x), z(x), w(x) {};
	vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};

	T& operator[](int i)
	{
		if (i == 0) return x;
		if (i == 1) return y;
		if (i == 2) return z;
		return w;
	}

	vec4<T> operator+(vec4<T> v) const
	{
		return vec4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	vec4<T> operator-(vec4<T> v) const
	{
		return vec4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	vec4<T> operator*(vec4<T> v) const
	{
		return vec4<T>(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	template <typename U>
	vec4<T> operator*(U s) const
	{
		return vec4<T>(x * s, y * s, z * s, w * s);
	}

	template <typename U>
	vec4<T> operator/(U f) const
	{
		float inv = (float)(1.0f / f);
		return vec4<T>(x * inv, y * inv, z * inv, w * inv);
	}
};

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const vec4<T>& v)
{
	os << "[ " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << " ]";
	return os;
}

// mat4x4 Declarations

template <typename T>
struct mat4x4
{
	T m[4][4];

	mat4x4()
	{
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		m[0][1] = m[0][2] = m[0][3] = 0;
		m[1][0] = m[1][2] = m[1][3] = 0;
		m[2][0] = m[2][1] = m[2][3] = 0;
		m[3][0] = m[3][1] = m[3][2] = 0;
	}

	mat4x4(
		T t00, T t01, T t02, T t03,
		T t10, T t11, T t12, T t13,
		T t20, T t21, T t22, T t23,
		T t30, T t31, T t32, T t33)
	{
		m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
		m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
		m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
		m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
	}

	vec4<T> operator[](int n) const
	{
		return vec4<T>(m[n][0], m[n][1], m[n][2], m[n][3]);
	}

	mat4x4<T> operator*(T s)
	{
		return mat4x4<T>(
			m[0][0] * s, m[0][1] * s, m[0][2] * s, m[0][3] * s,
			m[1][0] * s, m[1][1] * s, m[1][2] * s, m[1][3] * s, 
			m[2][0] * s, m[2][1] * s, m[2][2] * s, m[2][3] * s, 
			m[3][0] * s, m[3][1] * s, m[3][2] * s, m[3][3] * s);
	}
};

template <typename T>
inline mat4x4<T> transpose(const mat4x4<T>& m)
{
	mat4x4<T> _m;
	_m.m[0][0] = m.m[0][0];
	_m.m[0][1] = m.m[1][0];
	_m.m[0][2] = m.m[2][0];
	_m.m[0][3] = m.m[3][0];

	_m.m[1][0] = m.m[0][1];
	_m.m[1][1] = m.m[1][1];
	_m.m[1][2] = m.m[2][1];
	_m.m[1][3] = m.m[3][1];

	_m.m[2][0] = m.m[0][2];
	_m.m[2][1] = m.m[1][2];
	_m.m[2][2] = m.m[2][2];
	_m.m[2][3] = m.m[3][2];

	_m.m[3][0] = m.m[0][3];
	_m.m[3][1] = m.m[1][3];
	_m.m[3][2] = m.m[2][3];
	_m.m[3][3] = m.m[3][3];
	return _m;
}

template <typename T>
inline vec3<T> transform_direction(const mat4x4<T>& m, const vec3<T>& v)
{
	T x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z;
	T y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z;
	T z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z;

	return vec3<T>(x, y, z);
}

template <typename T>
inline vec3<T> transform_point(const mat4x4<T>& m, const vec3<T>& v)
{
	T x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
	T y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
	T z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];
	T w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3];

	return vec3<T>(x, y, z) / w;
}

template <typename T>
vec4<T> operator*(const mat4x4<T>& m, const vec4<T>& v)
{
	vec4<T> _v;
	_v.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
	_v.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
	_v.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
	_v.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;

	return _v;
}

template <typename T>
mat4x4<T> operator*(const mat4x4<T>& m1, const mat4x4<T>& m2)
{
	//for 4x4:
	// m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
	
	mat4x4<T> m;

	m.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[1][0] * m2.m[0][1] + m1.m[2][0] * m2.m[0][2] + m1.m[3][0] * m2.m[0][3];
	m.m[0][1] = m1.m[0][1] * m2.m[0][0] + m1.m[1][1] * m2.m[0][1] + m1.m[2][1] * m2.m[0][2] + m1.m[3][1] * m2.m[0][3];
	m.m[0][2] = m1.m[0][2] * m2.m[0][0] + m1.m[1][2] * m2.m[0][1] + m1.m[2][2] * m2.m[0][2] + m1.m[3][2] * m2.m[0][3];
	m.m[0][3] = m1.m[0][3] * m2.m[0][0] + m1.m[1][3] * m2.m[0][1] + m1.m[2][3] * m2.m[0][2] + m1.m[3][3] * m2.m[0][3];

	m.m[1][0] = m1.m[0][0] * m2.m[1][0] + m1.m[1][0] * m2.m[1][1] + m1.m[2][0] * m2.m[1][2] + m1.m[3][0] * m2.m[1][3];
	m.m[1][1] = m1.m[0][1] * m2.m[1][0] + m1.m[1][1] * m2.m[1][1] + m1.m[2][1] * m2.m[1][2] + m1.m[3][1] * m2.m[1][3];
	m.m[1][2] = m1.m[0][2] * m2.m[1][0] + m1.m[1][2] * m2.m[1][1] + m1.m[2][2] * m2.m[1][2] + m1.m[3][2] * m2.m[1][3];
	m.m[1][3] = m1.m[0][3] * m2.m[1][0] + m1.m[1][3] * m2.m[1][1] + m1.m[2][3] * m2.m[1][2] + m1.m[3][3] * m2.m[1][3];

	m.m[2][0] = m1.m[0][0] * m2.m[2][0] + m1.m[1][0] * m2.m[2][1] + m1.m[2][0] * m2.m[2][2] + m1.m[3][0] * m2.m[2][3];
	m.m[2][1] = m1.m[0][1] * m2.m[2][0] + m1.m[1][1] * m2.m[2][1] + m1.m[2][1] * m2.m[2][2] + m1.m[3][1] * m2.m[2][3];
	m.m[2][2] = m1.m[0][2] * m2.m[2][0] + m1.m[1][2] * m2.m[2][1] + m1.m[2][2] * m2.m[2][2] + m1.m[3][2] * m2.m[2][3];
	m.m[2][3] = m1.m[0][3] * m2.m[2][0] + m1.m[1][3] * m2.m[2][1] + m1.m[2][3] * m2.m[2][2] + m1.m[3][3] * m2.m[2][3];

	m.m[3][0] = m1.m[0][0] * m2.m[3][0] + m1.m[1][0] * m2.m[3][1] + m1.m[2][0] * m2.m[3][2] + m1.m[3][0] * m2.m[3][3];
	m.m[3][1] = m1.m[0][1] * m2.m[3][0] + m1.m[1][1] * m2.m[3][1] + m1.m[2][1] * m2.m[3][2] + m1.m[3][1] * m2.m[3][3];
	m.m[3][2] = m1.m[0][2] * m2.m[3][0] + m1.m[1][2] * m2.m[3][1] + m1.m[2][2] * m2.m[3][2] + m1.m[3][2] * m2.m[3][3];
	m.m[3][3] = m1.m[0][3] * m2.m[3][0] + m1.m[1][3] * m2.m[3][1] + m1.m[2][3] * m2.m[3][2] + m1.m[3][3] * m2.m[3][3];

	return m;
}

template <typename T>
inline mat4x4<T> inverse(const mat4x4<T>& m)
{
	T coef00 = m.m[2][2] * m.m[3][3] - m.m[3][2] * m.m[2][3];
	T coef02 = m.m[1][2] * m.m[3][3] - m.m[3][2] * m.m[1][3];
	T coef03 = m.m[1][2] * m.m[2][3] - m.m[2][2] * m.m[1][3];

	T coef04 = m.m[2][1] * m.m[3][3] - m.m[3][1] * m.m[2][3];
	T coef06 = m.m[1][1] * m.m[3][3] - m.m[3][1] * m.m[1][3];
	T coef07 = m.m[1][1] * m.m[2][3] - m.m[2][1] * m.m[1][3];

	T coef08 = m.m[2][1] * m.m[3][2] - m.m[3][1] * m.m[2][2];
	T coef10 = m.m[1][1] * m.m[3][2] - m.m[3][1] * m.m[1][2];
	T coef11 = m.m[1][1] * m.m[2][2] - m.m[2][1] * m.m[1][2];

	T coef12 = m.m[2][0] * m.m[3][3] - m.m[3][0] * m.m[2][3];
	T coef14 = m.m[1][0] * m.m[3][3] - m.m[3][0] * m.m[1][3];
	T coef15 = m.m[1][0] * m.m[2][3] - m.m[2][0] * m.m[1][3];

	T coef16 = m.m[2][0] * m.m[3][2] - m.m[3][0] * m.m[2][2];
	T coef18 = m.m[1][0] * m.m[3][2] - m.m[3][0] * m.m[1][2];
	T coef19 = m.m[1][0] * m.m[2][2] - m.m[2][0] * m.m[1][2];

	T coef20 = m.m[2][0] * m.m[3][1] - m.m[3][0] * m.m[2][1];
	T coef22 = m.m[1][0] * m.m[3][1] - m.m[3][0] * m.m[1][1];
	T coef23 = m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1];

	vec4<f32> fac0(coef00, coef00, coef02, coef03);
	vec4<f32> fac1(coef04, coef04, coef06, coef07);
	vec4<f32> fac2(coef08, coef08, coef10, coef11);
	vec4<f32> fac3(coef12, coef12, coef14, coef15);
	vec4<f32> fac4(coef16, coef16, coef18, coef19);
	vec4<f32> fac5(coef20, coef20, coef22, coef23);

	vec4<f32> cec0(m.m[1][0], m.m[0][0], m.m[0][0], m.m[0][0]);
	vec4<f32> cec1(m.m[1][1], m.m[0][1], m.m[0][1], m.m[0][1]);
	vec4<f32> cec2(m.m[1][2], m.m[0][2], m.m[0][2], m.m[0][2]);
	vec4<f32> cec3(m.m[1][3], m.m[0][3], m.m[0][3], m.m[0][3]);

	vec4<f32> inv0(cec1 * fac0 - cec2 * fac1 + cec3 * fac2);
	vec4<f32> inv1(cec0 * fac0 - cec2 * fac3 + cec3 * fac4);
	vec4<f32> inv2(cec0 * fac1 - cec1 * fac3 + cec3 * fac5);
	vec4<f32> inv3(cec0 * fac2 - cec1 * fac4 + cec2 * fac5);

	mat4x4<f32> _m(
		inv0.x, -inv0.y, inv0.z, -inv0.w,
		-inv1.x, inv1.y, -inv1.z, inv1.w,
		inv2.x, -inv2.y, inv2.z, -inv2.w,
		-inv3.x, inv3.y, -inv3.z, inv3.w);

	vec4<f32> row0(_m.m[0][0], _m.m[1][0], _m.m[2][0], _m.m[3][0]);
	vec4<f32> dot0(m[0] * row0);
	T idet = (dot0.x + dot0.y) + (dot0.z + dot0.w);
	return _m * (1.0f / idet);
}

template <typename T, typename U>
inline mat4x4<T> translate(const mat4x4<T>& m, U x, U y, U z)
{
	mat4x4<T> _m = m;
	_m.m[3][0] = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
	_m.m[3][1] = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
	_m.m[3][2] = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];
	return _m;
}

template <typename T>
inline mat4x4<T> translate(const mat4x4<T>& m, const vec3<T>& v)
{
	return translate(v.x, v.y, v.z);
}

template <typename T, typename U>
inline mat4x4<T> scale(const mat4x4<T>& m, U x, U y, U z)
{
	mat4x4<T> _m;
	_m.m[0][0] = m.m[0][0] * x;
	_m.m[0][1] = m.m[0][0] * x;
	_m.m[0][2] = m.m[0][0] * x;
	_m.m[0][3] = m.m[0][0] * x;

	_m.m[1][0] = m.m[0][0] * y;
	_m.m[1][1] = m.m[0][0] * y;
	_m.m[1][2] = m.m[0][0] * y;
	_m.m[1][3] = m.m[0][0] * y;

	_m.m[2][0] = m.m[0][0] * z;
	_m.m[2][1] = m.m[0][0] * z;
	_m.m[2][2] = m.m[0][0] * z;
	_m.m[2][3] = m.m[0][0] * z;

	_m.m[3][0] = m.m[3][0];
	_m.m[3][1] = m.m[3][1];
	_m.m[3][2] = m.m[3][2];
	_m.m[3][3] = m.m[3][3];
	return _m;
}

template <typename T>
inline mat4x4<T> scale(const mat4x4<T>& m, const vec3<T>& v)
{
	return scale(v.x, v.y, v.z);
}

template <typename T, typename U>
inline mat4x4<T> rotate(const mat4x4<T>& m, U angle, const vec3<T>& v)
{
	T a = angle;
	T c = cos(a);
	T s = sin(a);

	vec3<T> axis(normalize(v));
	vec3<T> temp((T(1) - c) * axis);

	mat4x4<T> r;
	r.m[0][0] = c + temp[0] * axis[0];
	r.m[0][1] = temp[0] * axis[1] + s * axis[2];
	r.m[0][2] = temp[0] * axis[2] - s * axis[1];

	r.m[1][0] = temp[1] * axis[0] - s * axis[2];
	r.m[1][1] = c + temp[1] * axis[1];
	r.m[1][2] = temp[1] * axis[2] + s * axis[0];

	r.m[2][0] = temp[2] * axis[0] + s * axis[1];
	r.m[2][1] = temp[2] * axis[1] - s * axis[0];
	r.m[2][2] = c + temp[2] * axis[2];

	mat4x4<T> _m;
	_m.m[0][0] = m.m[0][0] * r.m[0][0] + m.m[1][0] * r.m[0][1] + m.m[2][0] * r.m[0][2];
	_m.m[0][1] = m.m[0][1] * r.m[0][0] + m.m[1][1] * r.m[0][1] + m.m[2][1] * r.m[0][2];
	_m.m[0][2] = m.m[0][2] * r.m[0][0] + m.m[1][2] * r.m[0][1] + m.m[2][2] * r.m[0][2];
	_m.m[0][3] = m.m[0][3] * r.m[0][0] + m.m[1][3] * r.m[0][1] + m.m[2][3] * r.m[0][2];

	_m.m[1][0] = m.m[0][0] * r.m[1][0] + m.m[1][0] * r.m[1][1] + m.m[2][0] * r.m[1][2];
	_m.m[1][1] = m.m[0][1] * r.m[1][0] + m.m[1][1] * r.m[1][1] + m.m[2][1] * r.m[1][2];
	_m.m[1][2] = m.m[0][2] * r.m[1][0] + m.m[1][2] * r.m[1][1] + m.m[2][2] * r.m[1][2];
	_m.m[1][3] = m.m[0][3] * r.m[1][0] + m.m[1][3] * r.m[1][1] + m.m[2][3] * r.m[1][2];

	_m.m[2][0] = m.m[0][0] * r.m[2][0] + m.m[1][0] * r.m[2][1] + m.m[2][0] * r.m[2][2];
	_m.m[2][1] = m.m[0][1] * r.m[2][0] + m.m[1][1] * r.m[2][1] + m.m[2][1] * r.m[2][2];
	_m.m[2][2] = m.m[0][2] * r.m[2][0] + m.m[1][2] * r.m[2][1] + m.m[2][2] * r.m[2][2];
	_m.m[2][3] = m.m[0][3] * r.m[2][0] + m.m[1][3] * r.m[2][1] + m.m[2][3] * r.m[2][2];

	_m.m[3][0] = m.m[3][0];
	_m.m[3][1] = m.m[3][1];
	_m.m[3][2] = m.m[3][2];
	_m.m[3][3] = m.m[3][3];
	return _m;
}

template <typename T>
inline mat4x4<T> perspective(T fovy, T aspect, T zNear, T zFar)
{
	T tanHalfFovy = tan(fovy / 2.0f);

	mat4x4<T> m;
	m.m[0][0] = 1.0f / (aspect * tanHalfFovy);
	m.m[1][1] = 1.0f / (tanHalfFovy);
	m.m[2][2] = (zFar + zNear) / (zFar - zNear);
	m.m[2][3] = 1.0f;
	m.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	m.m[3][3] = 0.0f;
	return m;
}

template <typename T>
inline mat4x4<T> perspectiveRH(T fovy, T aspect, T zNear, T zFar)
{
	T tanHalfFovy = tan(fovy / 2.0f);

	mat4x4<T> m;
	m.m[0][0] = 1.0f / (aspect * tanHalfFovy);
	m.m[1][1] = 1.0f / (tanHalfFovy);
	m.m[2][2] = -(zFar + zNear) / (zFar - zNear);
	m.m[2][3] = -1.0f;
	m.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
	m.m[3][3] = 0.0f;
	return m;
}

template <typename T>
inline mat4x4<T> ortho(T left, T right, T bottom, T top, T zNear, T zFar)
{
	mat4x4<T> m;
	m.m[0][0] = 2.0f / (right - left);
	m.m[1][1] = 2.0f / (top - bottom);
	m.m[2][2] = 2.0f / (zFar - zNear);
	m.m[3][0] = -(right + left) / (right - left);
	m.m[3][1] = -(top + bottom) / (top - bottom);
	m.m[3][2] = -(zFar + zNear) / (zFar - zNear);
	return m;
}

template <typename T>
inline mat4x4<T> orthoRH(T left, T right, T bottom, T top, T zNear, T zFar)
{
	mat4x4<T> m;
	m.m[0][0] = 2.0f / (right - left);
	m.m[1][1] = 2.0f / (top - bottom);
	m.m[2][2] = -2.0f / (zFar - zNear);
	m.m[3][0] = -(right + left) / (right - left);
	m.m[3][1] = -(top + bottom) / (top - bottom);
	m.m[3][2] = -(zFar + zNear) / (zFar - zNear);
	return m;
}

template <typename T>
inline mat4x4<T> lookAt(const vec3<T>& eye, const vec3<T>& center, const vec3<T>& up)
{
	vec3<T> f(normalize(center - eye));
	vec3<T> s(normalize(cross(f, up)));
	vec3<T> u(cross(s, f));

	mat4x4<T> m;
	m.m[0][0] = s.x;
	m.m[1][0] = s.y;
	m.m[2][0] = s.z;

	m.m[0][1] = u.x;
	m.m[1][1] = u.y;
	m.m[2][1] = u.z;

	m.m[0][2] = -f.x;
	m.m[1][2] = -f.y;
	m.m[2][2] = -f.z;

	m.m[3][0] = -dot(s, eye);
	m.m[3][1] = -dot(u, eye);
	m.m[3][2] = -dot(f, eye); //left-handed=negative
	return m;
}

template <typename T>
inline mat4x4<T> lookAtRH(const vec3<T>& eye, const vec3<T>& center, const vec3<T>& up)
{
	vec3<T> f(normalize(center - eye));
	vec3<T> s(normalize(cross(f, up)));
	vec3<T> u(cross(s, f));

	mat4x4<T> m;
	m.m[0][0] = s.x;
	m.m[1][0] = s.y;
	m.m[2][0] = s.z;

	m.m[0][1] = u.x;
	m.m[1][1] = u.y;
	m.m[2][1] = u.z;

	m.m[0][2] = -f.x;
	m.m[1][2] = -f.y;
	m.m[2][2] = -f.z;

	m.m[3][0] = -dot(s, eye);
	m.m[3][1] = -dot(u, eye);
	m.m[3][2] = dot(f, eye);
	return m;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const mat4x4<T>& m)
{
	os << "[ [" << m.m[0][0] << ", " << m.m[0][1] << ", " << m.m[0][2] << ", " << m.m[0][3] << " ]\n"
		<< "  [" << m.m[1][0] << ", " << m.m[1][1] << ", " << m.m[1][2] << ", " << m.m[1][3] << " ]\n"
		<< "  [" << m.m[2][0] << ", " << m.m[2][1] << ", " << m.m[2][2] << ", " << m.m[2][3] << " ]\n"
		<< "  [" << m.m[3][0] << ", " << m.m[3][1] << ", " << m.m[3][2] << ", " << m.m[3][3] << " ] ]";
	return os;
}

// mat2x3 Declarations

template <typename T>
struct mat2x3
{
	T a, b, c, d, x, y;

	mat2x3()
	{
		a = 1; b = 0;
		c = 0; d = 1;
		x = 0; y = 0;
	}

	mat2x3(T a, T b, T c, T d, T x, T y) :
		a(a), b(b), c(c), d(d), x(x), y(y)
	{
	}
};

template <typename T, typename U>
inline mat2x3<T> translate(const mat2x3<T>& m, U dx, U dy)
{
	mat2x3<T> _m = m;
	_m.x += dx;
	_m.y += dy;
	return _m;
}

template <typename T, typename U>
inline mat2x3<T> scale(const mat2x3<T>& m, U sx, U sy)
{
	mat2x3<T> _m = m;
	_m.a *= sx;
	_m.b *= sy;
	_m.c *= sx;
	_m.d *= sy;
	_m.x *= sx;
	_m.y *= sy;
	return _m;
}

template <typename T, typename U>
inline mat2x3<T> rotate(const mat2x3<T>& m, U radians)
{
	mat2x3<T> _m = m;
	T cos = cosf(radians);
	T sin = sinf(radians);

	T a1 = m.a * cos - m.b * sin;
	_m.b = m.a * sin + m.b * cos;
	_m.a = a1;

	T c1 = m.c * cos - m.d * sin;
	_m.d = m.c * sin + m.d * cos;
	_m.c = c1;

	T tx1 = m.x * cos - m.y * sin;
	_m.y = m.x * sin + m.y * cos;
	_m.x = tx1;
	return _m;
}

template <typename T, typename U>
inline vec2<T> transform_point(const mat2x3<T>& m, U x, U y)
{
	return vec2<T>(x * m.a + y * m.c + m.x, x * m.b + y * m.d + m.y);
}

template <typename T>
inline vec2<T> transform_point(const mat2x3<T>& m, const vec2<T>& v)
{
	return vec2<T>(v.x * m.a + v.y * m.c + m.x, v.x * m.b + v.y * m.d + m.y);
}

template <typename T>
inline bool isConvexAngle(const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& p3)
{
	vec2<T> v1 = p2 - p1;
	vec2<T> v2 = p3 - p2;
	return cross(v1, v2) > 0;
}

template <typename T>
inline bool isPointInTriangle(const vec2<T>& p, const vec2<T>& a, const vec2<T>& b, const vec2<T>& c)
{
	vec2<T> pa = p - a;
	vec2<T> pb = p - b;
	vec2<T> pc = p - c;

	T t1 = cross(pa, pb);
	T t2 = cross(pb, pc);
	T t3 = cross(pc, pa);

	return t1 > 0 && t2 > 0 && t3 > 0 || t1 < 0 && t2 < 0 && t3 < 0;
}

// ray Declarations

struct ray
{
	vec3<f32> origin;

	vec3<f32> direction;

	f32 tMin = 0.0f;

	f32 tMax = F32_INF;
};

// aabb Declarations

struct aabb
{
	vec3<f32> min;

	vec3<f32> max;
};

#endif // KD_MATH_H
