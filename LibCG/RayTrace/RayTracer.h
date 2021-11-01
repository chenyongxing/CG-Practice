#pragma once

#include <functional>
#include "../Camera.h"
#include "Bvh.h"

enum struct RenderOutput
{
	Beaut,
	Albedo,
	Normal,
	Depth,
	Barycentric
};

namespace RayTracer
{
	extern Camera camera;

	extern BVHAccel bvhScene;

	extern RenderOutput renderOutput;

	extern int maxDepth;

	extern int samplesPerPixel;

	extern std::function<void(float)> renderProgressCallback;

	enum struct PTFlag
	{
		None,
		Terminate
	};

	struct Payload
	{
		u32 seed;
		vec3<f32> origin;
		vec3<f32> direction;
		vec3<f32> radiance;
		vec3<f32> attenuation;
		bool done = false;
		HitInfo hitInfo;
	};

	struct Param
	{
		u32 frameCount = 0;
		f32 cameraNdcXscale = 1.0f;
		f32 cameraNdcYscale = 1.0f;
		vec3<f32> cameraPosition;
		vec3<f32> cameraRight;
		vec3<f32> cameraUp;
		vec3<f32> cameraFront;
	};
}

void render(i32 width, i32 height, u32* buffer, bool parallel = true);

vec3<f32> ray_gen_single(i32 x, i32 y, i32 width, i32 height, const RayTracer::Param& param);

vec3<f32> ray_gen(i32 x, i32 y, i32 width, i32 height, const RayTracer::Param& param);

void trace_ray(ray& ray, const BVHAccel& scene, RayTracer::Payload& payload);

void closest_hit(const ray& ray, RayTracer::Payload& payload);

void miss_hit(const ray& ray, RayTracer::Payload& payload);

bool closest_hit_occlusion(const ray& ray);
