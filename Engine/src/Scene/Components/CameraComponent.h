#pragma once

#include "Scene/SceneCamera.h"

#include "cereal/cereal.hpp"

struct CameraComponent
{
	SceneCamera Camera;
	bool Primary = true;
	bool FixedAspectRatio = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const SceneCamera& camera, bool primaryCamera = true, bool fixedAspectRatio = true)
		:Camera(camera),
		Primary(primaryCamera),
		FixedAspectRatio(fixedAspectRatio) {}

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