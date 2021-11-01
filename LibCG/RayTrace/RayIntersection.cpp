#include "RayIntersection.h"

bool ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray)
{
	vec3<f32> tMin = (pmin - ray.origin) / ray.direction;
	vec3<f32> tMax = (pmax - ray.origin) / ray.direction;

	vec3<f32> t1 = min(tMin, tMax);
	vec3<f32> t2 = max(tMin, tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	return tFar >= tNear;
}

void ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray, f32& t)
{
	vec3<f32> tMin = (pmin - ray.origin) / ray.direction;
	vec3<f32> tMax = (pmax - ray.origin) / ray.direction;

	vec3<f32> t1 = min(tMin, tMax);
	vec3<f32> t2 = max(tMin, tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	if (tFar >= tNear)
		t = tNear > 0 ? tNear : tFar;
	else
		t = F32_INF;
}

void ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray, f32& t, vec3<f32>& normal)
{
	vec3<f32> tMin = (pmin - ray.origin) / ray.direction;
	vec3<f32> tMax = (pmax - ray.origin) / ray.direction;

	vec3<f32> t1 = min(tMin, tMax);
	vec3<f32> t2 = max(tMin, tMax);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	if (tFar >= tNear)
	{
		t = tNear > 0 ? tNear : tFar;
		normal = equal(t1, vec3<f32>(tNear)) * sign(ray.direction * -1);
	}
	else
		t = F32_INF;
}

void ray_sphere_intersect(const vec3<f32>& center, f32 radius, const ray& ray, f32& t)
{
	vec3<f32> oc = ray.origin - center;
	f32 a = dot(ray.direction, ray.direction);
	f32 b = 2.0f * dot(oc, ray.direction);
	f32 c = dot(oc, oc) - radius * radius;
	f32 discriminant = b * b - 4 * a * c;

	if (discriminant >= 0)
	{
		t = (-b - sqrt(discriminant)) / (2.0f * a);
		vec3<f32> hit_pos = ray.origin + ray.direction * t;
	}
	else
		t = F32_INF;
}

void ray_sphere_intersect(const vec3<f32>& center, f32 radius, const ray& ray, f32& t, vec3<f32>& normal)
{
	vec3<f32> oc = ray.origin - center;
	f32 a = dot(ray.direction, ray.direction);
	f32 b = 2.0f * dot(oc, ray.direction);
	f32 c = dot(oc, oc) - radius * radius;
	f32 discriminant = b * b - 4 * a * c;

	if (discriminant >= 0)
	{
		t = (-b - sqrt(discriminant)) / (2.0f * a);
		vec3<f32> hit_pos = ray.origin + ray.direction * t;
		normal = normalize(hit_pos - center);
	}
	else
		t = F32_INF;
}

void ray_triangle_intersect(const vec3<f32>& v0, const vec3<f32>& v1, const vec3<f32>& v2, 
	const ray& ray, f32& t, vec3<f32>& bary)
{
	vec3<f32> e0 = v1 - v0;
	vec3<f32> e1 = v2 - v0;
	vec3<f32> pv = cross(ray.direction, e1);
	f32 det = dot(e0, pv);

	vec3<f32> tv = ray.origin - v0;
	vec3<f32> qv = cross(tv, e0);

	vec3<f32> uvt;
	uvt.x = dot(tv, pv);
	uvt.y = dot(ray.direction, qv);
	uvt.z = dot(e1, qv);
	uvt = uvt / det;

	f32 w = 1.0f - uvt.x - uvt.y;
	if (uvt.x >= 0 && uvt.y >= 0 && w >= 0 && uvt.z >= 0)
	{
		t = uvt.z;
		bary.x = uvt.x;
		bary.y = uvt.y;
		bary.z = w;
	}
	else
		t = F32_INF;
}

void ray_triangle_intersect(const vec3<f32>& v0, const vec3<f32>& v1, const vec3<f32>& v2, 
	const ray& ray, f32& t, vec3<f32>& bary, vec3<f32>& normal)
{
	vec3<f32> e0 = v1 - v0;
	vec3<f32> e1 = v2 - v0;
	vec3<f32> pv = cross(ray.direction, e1);
	f32 det = dot(e0, pv);

	vec3<f32> tv = ray.origin - v0;
	vec3<f32> qv = cross(tv, e0);

	vec3<f32> uvt;
	uvt.x = dot(tv, pv);
	uvt.y = dot(ray.direction, qv);
	uvt.z = dot(e1, qv);
	uvt = uvt / det;

	f32 w = 1.0f - uvt.x - uvt.y;
	if (uvt.x >= 0 && uvt.y >= 0 && w >= 0 && uvt.z >= 0)
	{
		t = uvt.z;
		bary.x = uvt.x;
		bary.y = uvt.y;
		bary.z = w;
		normal = normalize(cross(v1 - v0, v2 - v0));
	}
	else
		t = F32_INF;
}
