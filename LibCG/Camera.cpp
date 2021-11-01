#include "Camera.h"

void Camera::lookAt(const vec3<f32>& target)
{
	view = ::lookAt(getPositon(), target, vec3<f32>(0, 1, 0));
	transform = inverse(view);
}

void Camera::lookAt(const vec3<f32>& target, const vec3<f32>& up)
{
	view = ::lookAt(getPositon(), target, up);
	transform = inverse(view);
}

void Camera::setClip(f32 near, f32 far)
{
	m_near = near;
	m_far = far;
}

void Camera::setFovYAndAspect(f32 fov, f32 aspect)
{
	isOrtho = false;
	m_fovY = fov;
	m_aspect = aspect;
	projection = ::perspective(::radians(m_fovY), m_aspect, m_near, m_far);
}

void Camera::setFovY(f32 fov)
{
	isOrtho = false;
	m_fovY = fov;
	projection = ::perspective(::radians(m_fovY), m_aspect, m_near, m_far);
}

void Camera::setOrthoSize(f32 width, f32 height)
{
	isOrtho = true;
	m_aspect = width / height;
	m_orthoHeight = height;
	projection = ::ortho(width * -0.5f, width * 0.5f, height * -0.5f, height * 0.5f, m_near, m_far);
}

void Camera::getBasisVectors(vec3<f32>& right, vec3<f32>& up, vec3<f32>& front)
{
	right = vec3<f32>(transform.m[0][0], transform.m[1][0], transform.m[2][0]);
	up = vec3<f32>(transform.m[0][1], transform.m[1][1], transform.m[2][1]);
	front = vec3<f32>(transform.m[0][2], transform.m[1][2], transform.m[2][2]);
}

void Camera::moveRight(f32 delta)
{
	transform = translate(transform, delta, 0.f, 0.f);
	view = inverse(transform);
}

void Camera::moveUp(f32 delta)
{
	transform = translate(transform, 0.f, delta, 0.f);
	view = inverse(transform);
}

void Camera::moveFoward(f32 delta)
{
	transform = translate(transform, 0.f, 0.f, delta);
	view = inverse(transform);
}

void Camera::rotate(f32 yaw, f32 pitch)
{
	vec3<f32> up = vec3<f32>(0, 1, 0);
	up = transform_direction(view, up);
	transform = ::rotate(transform, ::radians(yaw), up);
	transform = ::rotate(transform, ::radians(pitch), vec3<f32>(1, 0, 0));
	view = inverse(transform);
}
