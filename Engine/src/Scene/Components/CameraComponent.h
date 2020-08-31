#pragma once

#include "Scene/SceneCamera.h"

struct CameraComponent
{
	SceneCamera Camera;
	bool Primary = true;
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const SceneCamera& camera, bool mainCamera = true, bool fixedAspectRatio = true)
		:Camera(camera){}

	operator SceneCamera& () { return Camera; }
	operator const SceneCamera& () const { return Camera; }
};