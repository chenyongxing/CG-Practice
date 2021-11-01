#pragma once

#include "../KDMath.h"

bool ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray);

void ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray, f32& t);

void ray_aabb_intersect(const vec3<f32>& pmin, const vec3<f32>& pmax, const ray& ray, f32& t, vec3<f32>& normal);

void ray_sphere_intersect(const vec3<f32>& center, f32 radius, const ray& ray, f32& t);

void ray_sphere_intersect(const vec3<f32>& center, f32 radius, const ray& ray, f32& t, vec3<f32>& normal);

void ray_triangle_intersect(const vec3<f32>& v0, const vec3<f32>& v1, const vec3<f32>& v2, const ray& ray, f32& t, vec3<f32>& bary);

void ray_triangle_intersect(const vec3<f32>& v0, const vec3<f32>& v1, const vec3<f32>& v2, const ray& ray, f32& t, vec3<f32>& bary, vec3<f32>& normal);
