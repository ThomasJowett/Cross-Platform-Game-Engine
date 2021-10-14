#pragma once

#include "cereal/cereal.hpp"

struct RigidBody2DComponent
{
	enum class BodyType 
	{
		STATIC = 0,
		KINEMATIC = 1,
		DYNAMIC = 2,
	};

	BodyType Type = BodyType::STATIC;
	bool FixedRotation = false;

	void* RuntimeBody = nullptr;

	RigidBody2DComponent() = default;
	RigidBody2DComponent(const RigidBody2DComponent&) = default;
	RigidBody2DComponent(BodyType type, bool fixedRotation)
		:Type(type), FixedRotation(fixedRotation) {}
	
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("BodyType", Type));
		archive(cereal::make_nvp("FixedRotation", FixedRotation));
	}
};