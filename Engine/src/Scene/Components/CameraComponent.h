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

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("SceneCamera", Camera));
		archive(cereal::make_nvp("Primary", Primary));
		archive(cereal::make_nvp("FixedAspectRatio", FixedAspectRatio));
	}
};