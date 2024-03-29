#pragma once

#include "cereal/cereal.hpp"

class b2Body;
class b2World;
class Entity;

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

	b2Body* runtimeBody = nullptr;

	RigidBody2DComponent() = default;
	RigidBody2DComponent(const RigidBody2DComponent&) = default;
	RigidBody2DComponent(BodyType type, bool fixedRotation)
		:type(type), fixedRotation(fixedRotation) {}

	void ApplyImpulse(Vector2f impulse);
	void ApplyImpulseAtPoint(Vector2f impulse, Vector2f center);
	void ApplyForce(Vector2f force);
	void ApplyForceAtPoint(Vector2f force, Vector2f center);
	void ApplyTorque(float torque);

	void SetLinearVelocity(Vector2f velocity);
	Vector2f GetLinearVelocity();

	void SetAngularVelocity(float velocity);
	float GetAngularVelocity();

	void SetTransform(const Vector2f& position, const float& angle);
	void GetTransform(Vector2f& position, float& rotation);

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(type, fixedRotation, gravityScale, angularDamping, linearDamping);
	}
};