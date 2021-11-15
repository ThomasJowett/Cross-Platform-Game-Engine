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

	BodyType type = BodyType::STATIC;
	bool fixedRotation = false;
	float gravityScale = 1.0f;
	float angularDamping = 0.0f;
	float linearDamping = 0.0f;

	void* RuntimeBody = nullptr;

	RigidBody2DComponent() = default;
	RigidBody2DComponent(const RigidBody2DComponent&) = default;
	RigidBody2DComponent(BodyType type, bool fixedRotation)
		:type(type), fixedRotation(fixedRotation) {}
	
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("BodyType", type));
		archive(cereal::make_nvp("FixedRotation", fixedRotation));
		archive(cereal::make_nvp("GravityScale", gravityScale));
		archive(cereal::make_nvp("AngularDamping", angularDamping));
		archive(cereal::make_nvp("LinearDamping", linearDamping));
	}
};