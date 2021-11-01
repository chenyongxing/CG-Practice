#pragma once

#include "../KDMath.h"

enum struct MaterialType
{
	Lambert,
	Mirror,
	Glass,
	Disney
};

struct Material
{
	MaterialType type = MaterialType::Lambert;
	vec3<f32> color{ 0.8f, 0.8f, 0.8f };
	vec3<f32> emissive;
	f32 metallic = 0.0f;
	f32 roughness = 0.1f;
};

struct HitInfo
{
	f32 t = F32_INF;
	vec3<f32> bary;
	vec3<f32> normal;
	Material material;
};

struct Primitive
{
	aabb aabb;
	Material material;

	virtual void updateAabb() = 0;
	virtual bool rayIntersect(const ray& ray, HitInfo& hitInfo) = 0;
};

struct PrimitiveAabox : public Primitive
{
	void updateAabb() override;
	bool rayIntersect(const ray& ray, HitInfo& hitInfo) override;
};

struct PrimitiveSphere : public Primitive
{
	vec3<f32> center;
	f32 radius = 1.0f;

	void updateAabb() override;
	bool rayIntersect(const ray& ray, HitInfo& hitInfo) override;
};

struct PrimitiveTriangle : public Primitive
{
	vec3<f32> vertex[3];

	void updateAabb() override;
	bool rayIntersect(const ray& ray, HitInfo& hitInfo) override;
};
