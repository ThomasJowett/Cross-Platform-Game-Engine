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

	operator SceneCamera& () { return Camera; }
	operator const SceneCamera& () const { return Camera; }
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("SceneCamera", Camera));
		archive(cereal::make_nvp("Primary", Primary));
		archive(cereal::make_nvp("FixedAspectRatio", FixedAspectRatio));
	}
};