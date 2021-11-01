#pragma once

#include <chrono>
#include "KDMath.h"

struct Profiler
{
	Profiler(std::string operation);
	~Profiler();
	std::string name;
	std::chrono::steady_clock::time_point start;
};

inline u32 rgb2hex(i32 r, i32 g, i32 b)
{
	return (r << 16) | (g << 8) | b;
}

inline u32 rgb2hex(vec3<f32> color)
{
	i32 r = (i32)(color.x * 255.f);
	i32 g = (i32)(color.y * 255.f);
	i32 b = (i32)(color.z * 255.f);

	return (r << 16) | (g << 8) | b;
}

inline u32 rgba2hex(i32 r, i32 g, i32 b, i32 a)
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

inline vec4<f32> hex2rgba(u32 color)
{
	float r = ((color >> 24) & 0xff) / 255.0f;
	float g = ((color >> 16) & 0xff) / 255.0f;
	float b = ((color >> 8) & 0xff) / 255.0f;
	float a = ((color) & 0xff) / 255.0f;

	return vec4<f32>(r, g, b, a);
}

/**
* Mersenne Twister algorithm
*/
template<u32 N = 4>
u32 rnd_init(u32 v0, u32 v1)
{
	u32 s0 = 0;

	for (u32 n = 0; n < N; n++)
	{
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}

	return v0;
}

/**
* Generate random uint32_t in [0, 2^24)
*/
inline u32 lcg(u32& prev)
{
	const u32 LCG_A = 1664525u;
	const u32 LCG_C = 1013904223u;
	prev = (LCG_A * prev + LCG_C);
	return prev & 0x00FFFFFF;
}

/**
* Generate random float in [0, 1)
*/
inline f32 rnd(u32& prev)
{
	return ((f32)lcg(prev) / (f32)0x01000000);
}

/**
* tangent space_to_world space
*/
vec3<f32> tangent_to_world(const vec3<f32>& dir, const vec3<f32>& normal);
