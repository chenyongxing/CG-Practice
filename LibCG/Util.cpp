#include "Util.h"

Profiler::Profiler(std::string name) : name(name)
{
	start = std::chrono::steady_clock::now();
}

Profiler::~Profiler()
{
	auto stop = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = stop - start;
	printf("[Profiler] (%s) cost %f seconds.\n", name.c_str(), elapsed_seconds.count());
}

vec3<f32> tangent_to_world(const vec3<f32>& dir, const vec3<f32>& normal)
{
	f32 sign = normal.z >= 0.0f ? 1.0f : -1.0f;
	f32 a = -1.0f / (sign + normal.z);
	f32 b = normal.x * normal.y * a;
	vec3<f32> tangent = { 1 + sign * a * normal.x * normal.x, sign * b, -sign * normal.x };
	vec3<f32> binormal = { b, sign + a * normal.y * normal.y, -normal.y };

	return dir.x * tangent + dir.y * binormal + dir.z * normal;
}
