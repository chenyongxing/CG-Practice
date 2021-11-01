#include "RayTracer.h"
#include "../Util.h"
#include "RayIntersection.h"
#include "Sampling.h"

#include <vector>
#include <algorithm>
#include <execution>
#include <atomic>

namespace RayTracer
{
	Camera camera;

	BVHAccel bvhScene;

	RenderOutput renderOutput = RenderOutput::Beaut;

	int maxDepth = 4;

	int samplesPerPixel = 64;

	static f32 RENDER_PROGRESS = 0.0f;
	std::function<void(float)> renderProgressCallback;
}

using namespace RayTracer;

void render(i32 width, i32 height, u32* buffer, bool parallel)
{
	Param param;
	param.cameraPosition = camera.getPositon();
	param.cameraNdcYscale = tan(radians(camera.getFovY()));
	param.cameraNdcXscale = param.cameraNdcYscale * camera.getAspect();
	camera.getBasisVectors(param.cameraRight, param.cameraUp, param.cameraFront);
	//if right-handed local front vector = -1
	//param.cameraFront = transform_direction(camera.getWorldMatrix(), vec3<f32>(0, 0, -1));

	RENDER_PROGRESS = 0.0f;
	std::atomic<int> render_count(0);
	auto renderPixel = [&](i32 i, i32 j)
	{
		if (renderOutput == RenderOutput::Beaut)
		{
			vec3<f32> hdr_color = ray_gen(i, j, width, height, param);
			buffer[j * width + i] = rgb2hex(hdr_color);
		}
		else
		{
			buffer[j * width + i] = rgb2hex(ray_gen_single(i, j, width, height, param));
		}

		//buffer[j * width + i] = rgb2hex(255 * u, 255 * v, 0);

		render_count++;
		f32 progress = render_count / float(width * height);
		if (progress - RENDER_PROGRESS > 0.001f || progress == 1.0f)
		{
			RENDER_PROGRESS = progress;
			if (renderProgressCallback)
			{
				renderProgressCallback(RENDER_PROGRESS * 100.f);
			}
		}
	};

	if (parallel)
	{
		Profiler profiler("render parallel");

		std::vector<i32> pixelIndices;
		pixelIndices.resize(width * height);
		for (int i = 0; i < width * height; i++)
			pixelIndices[i] = i;
		std::for_each(std::execution::par_unseq, pixelIndices.begin(), pixelIndices.end(),
			[&](auto&& index)
			{
				renderPixel(index % width, index / width);
			});
	}
	else
	{
		Profiler profiler("render");
		for (i32 j = height - 1; j >= 0; j--)
		{
			for (i32 i = 0; i < width; i++)
			{
				renderPixel(i, j);
			}
		}
	}
}

vec3<f32> ray_gen_single(i32 x, i32 y, i32 width, i32 height, const RayTracer::Param& param)
{
	f32 u = (f32(x) + 0.5f) / (width - 1);
	f32 v = (f32(y) + 0.5f) / (height - 1);

	vec2<f32> uv(u, v);
	//NDC [[0, 0], [1, 1]] -> [[-1, -1], [1, 1]]
	uv.x = uv.x * 2.0f - 1.0f;
	uv.y = uv.y * 2.0f - 1.0f;
	//perspective fov relative
	uv.x *= param.cameraNdcXscale;
	uv.y *= param.cameraNdcYscale; // NDC top-left=negative

	ray ray;
	ray.origin = param.cameraPosition;
	//view space vector to world space
	ray.direction = param.cameraRight * uv.x + param.cameraUp * uv.y + param.cameraFront;
	ray.direction = normalize(ray.direction);

	HitInfo hitInfo;
	bvhScene.rayIntersect(ray, hitInfo);

	if (hitInfo.t < F32_INF)
	{
		if (renderOutput == RenderOutput::Albedo)
		{
			return hitInfo.material.color;
		}
		else if (renderOutput == RenderOutput::Normal)
		{
			vec3<f32> N = hitInfo.normal;
			return 0.5f * vec3<f32>(N.x + 1, N.y + 1, N.z + 1);
		}
		else if (renderOutput == RenderOutput::Barycentric)
		{
			return hitInfo.bary;
		}
		else
		{
			return vec3<f32>(1, 0, 1);
		}
	}
	else
	{
		return vec3<f32>(0.1f);
	}
}

vec3<f32> ray_gen(i32 x, i32 y, i32 width, i32 height, const Param& param)
{
	int sppCount = samplesPerPixel;
	vec3<f32> result;
	do
	{
		u32 rnd_seed = rnd_init(x + y * width, sppCount); //param.frameCount
		
		//亚像素内抖动抗锯齿
		vec2<f32> subpixel_jitter(rnd(rnd_seed), rnd(rnd_seed));
		f32 u = (f32(x) + subpixel_jitter.x) / (width - 1);
		f32 v = (f32(y) + subpixel_jitter.y) / (height - 1);

		vec2<f32> uv(u, v);
		uv.x = uv.x * 2.0f - 1.0f;
		uv.y = uv.y * 2.0f - 1.0f;
		uv.x *= param.cameraNdcXscale;
		uv.y *= param.cameraNdcYscale;

		ray ray;
		ray.origin = param.cameraPosition;
		ray.direction = normalize(param.cameraRight * uv.x + param.cameraUp * uv.y + param.cameraFront);

		Payload payload;
		payload.seed = rnd_seed;
		payload.radiance = vec3<f32>(0.0f);
		payload.attenuation = vec3<f32>(1.0f);
		payload.done = false;

		for (int depth = 0; depth < maxDepth; depth++)
		{
			payload.radiance = vec3<f32>(0.0f);

			trace_ray(ray, bvhScene, payload);

			result += payload.attenuation * payload.radiance;

			if (payload.done)
				break;

			//RUSSIAN_ROULETTE

			ray.origin = payload.origin;
			ray.direction = payload.direction;
		}
	} while (--sppCount > 0);

	return result / (f32)samplesPerPixel;
}

void trace_ray(ray& ray, const BVHAccel& scene, Payload& payload)
{
	HitInfo hitInfo;
	if (scene.rayIntersect(ray, hitInfo))
	{
		payload.hitInfo = hitInfo;
	}

	if (hitInfo.t < F32_INF)
	{
		closest_hit(ray, payload);
		return;
	}
	miss_hit(ray, payload);
}

void closest_hit(const ray& ray, Payload& payload)
{
	vec3<f32> N = payload.hitInfo.normal;
	vec3<f32> P = ray.origin + ray.direction * payload.hitInfo.t;
	vec3<f32> ffnormal = faceforward(ray.direction * -1, N);
	vec3<f32> wi;
	f32 pdf;
	cosine_sample_hemisphere(rnd(payload.seed), rnd(payload.seed), wi, pdf);
	wi = tangent_to_world(wi, ffnormal);

	payload.attenuation *= payload.hitInfo.material.color;
	payload.radiance += payload.hitInfo.material.emissive;
	payload.direction = wi;
	payload.origin = P + ffnormal * 0.01f;
}

void miss_hit(const ray& ray, Payload& payload)
{
	auto t = 0.5f * (ray.direction.y + 1.0f);
	payload.radiance = (1.0f - t) * vec3<f32>(1.0f, 1.0f, 1.0f) + t * vec3<f32>(0.5f, 0.7f, 1.0f);
	payload.done = true;
}

bool closest_hit_occlusion(const ray& ray)
{
	return false;
}
