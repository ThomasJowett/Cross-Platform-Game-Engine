#pragma once

#include "Renderer/Camera.h"

struct CameraComponent
{
	Camera m_Camera;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const Camera& camera)
		:m_Camera(camera){}

	operator Camera& () { return m_Camera; }
	operator const Camera& () const { return m_Camera; }
};