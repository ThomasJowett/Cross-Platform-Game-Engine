#pragma once

#include "Scene/SceneCamera.h"

#include "cereal/cereal.hpp"

struct CameraComponent
{
	SceneCamera camera;
	bool primary = true;
	bool fixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;

	operator SceneCamera& () { return camera; }
	operator const SceneCamera& () const { return camera; }
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(camera, primary, fixedAspectRatio);
	}
};