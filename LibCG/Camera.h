#pragma once

#include "KDMath.h"

class Camera
{
public:
	void lookAt(const vec3<f32>& target);

	void lookAt(const vec3<f32>& target, const vec3<f32>& up);

	void setClip(f32 near, f32 far);

	void setFovYAndAspect(f32 fov, f32 aspect);

	void setFovY(f32 fov);

	void setOrthoSize(f32 width, f32 height);

	void getBasisVectors(vec3<f32>& right, vec3<f32>& up, vec3<f32>& front);

	void moveRight(f32 delta);

	void moveUp(f32 delta);

	void moveFoward(f32 delta);

	void rotate(f32 yaw, f32 pitch);

	inline bool ortho() { return isOrtho; }

	inline f32 getAspect() { return m_aspect; }

	inline f32 getFovY() { return m_fovY; }

	inline void setPositon(f32 x, f32 y, f32 z)
	{
		transform.m[3][0] = x;
		transform.m[3][1] = y;
		transform.m[3][2] = z;
		view = inverse(transform);
	}

	inline vec3<f32> getPositon()
	{
		return vec3<f32>(transform.m[3][0], transform.m[3][1], transform.m[3][2]);
	}

	inline mat4x4<f32>& getWorldMatrix()
	{
		return transform;
	};

	inline mat4x4<f32>& getViewMatrix() 
	{
		return view;
	};

	inline mat4x4<f32>& getProjectionMatrix()
	{
		return projection;
	};

private:
	mat4x4<f32> transform;
	mat4x4<f32> view;
	mat4x4<f32> projection;

	bool isOrtho = false;
	f32 m_aspect = 1.0f;
	f32 m_fovY = 45.0f;
	f32 m_near = 0.1f;
	f32 m_far = 100.0f;
	f32 m_orthoHeight = 1.0f;
};
