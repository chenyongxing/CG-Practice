#pragma once

#include <vector>
#include "Primitive.h"

struct BVHNode
{
	aabb aabb;
	//is_leaf
	Primitive* primitive = nullptr;

	BVHNode* left = nullptr;
	BVHNode* right = nullptr;

	bool rayIntersect(const ray& ray, HitInfo& hitInfo);
};

enum struct BVHAccelMode
{ 
	None, 
	Middle, 
	SAH
};

struct BVHAccel
{
	BVHAccelMode mode = BVHAccelMode::Middle;
	BVHNode* root = nullptr;
	std::vector<Primitive*> primitives;

	bool loadFormObj(const char* filename);
	bool loadFormVox(const char* filename);
	void build();
	BVHNode* buildRecursive(size_t start, size_t end);
	bool rayIntersect(const ray& ray, HitInfo& hitInfo) const;
};
