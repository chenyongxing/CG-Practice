#include "Primitive.h"
#include "RayIntersection.h"

void PrimitiveAabox::updateAabb()
{
}

bool PrimitiveAabox::rayIntersect(const ray& ray, HitInfo& hitInfo)
{
	ray_aabb_intersect(aabb.min, aabb.max, ray, hitInfo.t, hitInfo.normal);
	hitInfo.material = material;
	return hitInfo.t < F32_INF;
}

void PrimitiveSphere::updateAabb()
{
	auto rv = vec3<f32>(radius, radius, radius);
	aabb.min = center - rv;
	aabb.max = center + rv;
}

bool PrimitiveSphere::rayIntersect(const ray& ray, HitInfo& hitInfo)
{
	hitInfo.material = material;
	ray_sphere_intersect(center, radius, ray, hitInfo.t, hitInfo.normal);
	return hitInfo.t < F32_INF;
}

void PrimitiveTriangle::updateAabb()
{
	aabb.min = min(vertex[0], min(vertex[1], vertex[2]));
	aabb.max = max(vertex[0], max(vertex[1], vertex[2]));
}

bool PrimitiveTriangle::rayIntersect(const ray& ray, HitInfo& hitInfo)
{
	hitInfo.material = material;
	ray_triangle_intersect(vertex[0], vertex[1], vertex[2], ray, hitInfo.t, hitInfo.bary, hitInfo.normal);
	return hitInfo.t < F32_INF;
}
