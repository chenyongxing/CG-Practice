#include "Bvh.h"
#include "RayIntersection.h"
#include "../ModelLoader.h"
#include <algorithm>

bool BVHNode::rayIntersect(const ray& ray, HitInfo& hitInfo)
{
	if (ray_aabb_intersect(aabb.min, aabb.max, ray))
	{
		if (primitive)
		{
			return primitive->rayIntersect(ray, hitInfo);
		}
		else
		{
			HitInfo left_hit, right_hit;
			bool left_inter = left->rayIntersect(ray, left_hit);
			bool righ_intert = right->rayIntersect(ray, right_hit);

			//closest hit
			if (left_inter && righ_intert)
			{
				if (left_hit.t < right_hit.t)
					hitInfo = left_hit;
				else
					hitInfo = right_hit;
				return true;
			}
			else if (left_inter)
			{
				hitInfo = left_hit;
				return true;
			}
			else if (righ_intert)
			{
				hitInfo = right_hit;
				return true;
			}
			else
				return false;
		}
	}
	else
		return false;
}

bool BVHAccel::loadFormObj(const char* filename)
{
	ObjLoader loader;
	return loader.loadPrimitive(filename, primitives);
}

bool BVHAccel::loadFormVox(const char* filename)
{
	VoxLoader loader;
	return loader.loadPrimitive(filename, primitives);

	//PrimitiveAabox* primAabox = new PrimitiveAabox();
	//primAabox->aabb.min = { 0,0,0 };
	//primAabox->aabb.max = { 1,1,1 };
	//primitives.push_back(primAabox);
	return true;
}

void BVHAccel::build()
{
	if (primitives.size() < 3)
		return;

	root = buildRecursive(0, primitives.size() - 1);
}

static bool box_x_compare(const Primitive* a, const Primitive* b)
{
	return a->aabb.min.x < b->aabb.min.x;
}

static bool box_y_compare(const Primitive* a, const Primitive* b)
{
	return a->aabb.min.y < b->aabb.min.y;
}

static bool box_z_compare(const Primitive* a, const Primitive* b)
{
	return a->aabb.min.z < b->aabb.min.z;
}

BVHNode* BVHAccel::buildRecursive(size_t start, size_t end)
{
	BVHNode* node = new BVHNode();

	//随机选轴->3轴中跨度最大
	int axis = int(3 * rand() / RAND_MAX);

	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare : box_z_compare;

	size_t primNum = end - start;
	if (primNum == 1)
	{
		node->left = new BVHNode();
		node->left->primitive = primitives[start];
		node->left->aabb = node->left->primitive->aabb;

		node->right = new BVHNode();
		node->right->primitive = primitives[start];
		node->right->aabb = node->right->primitive->aabb;
	}
	else if (primNum == 2)
	{
		if (comparator(primitives[start], primitives[start + 1]))
		{
			node->left = new BVHNode();
			node->left->primitive = primitives[start];
			node->left->aabb = node->left->primitive->aabb;

			node->right = new BVHNode();
			node->right->primitive = primitives[start + 1];
			node->right->aabb = node->right->primitive->aabb;
		}
		else 
		{
			node->left = new BVHNode();
			node->left->primitive = primitives[start + 1];
			node->left->aabb = node->left->primitive->aabb;

			node->right = new BVHNode();
			node->right->primitive = primitives[start];
			node->right->aabb = node->right->primitive->aabb;
		}
	}
	else
	{
		std::sort(primitives.begin() + start, primitives.begin() + end, comparator);

		size_t mid = start + primNum / 2;
		node->left = buildRecursive(start, mid);
		node->right = buildRecursive(mid, end);
	}

	node->aabb.min = min(node->left->aabb.min, node->right->aabb.min);
	node->aabb.max = max(node->left->aabb.max, node->right->aabb.max);

	return node;
}

bool BVHAccel::rayIntersect(const ray& ray, HitInfo& hitInfo) const
{
	if (mode == BVHAccelMode::None)
	{
		HitInfo _hitInfo;
		for (auto& prim : primitives)
		{
			prim->rayIntersect(ray, _hitInfo);
			if (_hitInfo.t < hitInfo.t)
				hitInfo = _hitInfo;
		}
		return hitInfo.t < F32_INF;
	}
	else if (mode == BVHAccelMode::Middle)
	{
		return root->rayIntersect(ray, hitInfo);
	}
	else
		return false;
}
