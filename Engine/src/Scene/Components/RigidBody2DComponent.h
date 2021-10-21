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
	float GravityScale = 1.0f;
	float AngularDamping = 0.0f;
	float LinearDamping = 0.0f;

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
		archive(cereal::make_nvp("GravityScale", GravityScale));
		archive(cereal::make_nvp("AngularDamping", AngularDamping));
		archive(cereal::make_nvp("LinearDamping", LinearDamping));
	}
};