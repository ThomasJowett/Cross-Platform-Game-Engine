#pragma once

#include "cereal/cereal.hpp"
#include "Scripting/Lua/LuaBindings.h"

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

	REFLECT_LUA_BEGIN(RigidBody2DComponent)
		state.new_enum("BodyType", std::initializer_list<std::pair<sol::string_view, int>>{
			{ "Static", (int)BodyType::STATIC },
			{ "Kinematic", (int)BodyType::KINEMATIC },
			{ "Dynamic", (int)BodyType::DYNAMIC }
		});
		REFLECT_LUA_PROPERTY_CUSTOM("Type", "BodyType.Static, BodyType.Kinematic or BodyType.Dynamic", "BodyType",
			([](Self& c) { return c.type; }),
			([](Self& c, BodyType v) { c.type = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("FixedRotation", "Stop the body from rotating", "boolean",
			([](Self& c) { return c.fixedRotation; }),
			([](Self& c, bool v) { c.fixedRotation = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("GravityScale", "Multiplier on the scene's gravity for this body", "number",
			([](Self& c) { return c.gravityScale; }),
			([](Self& c, float v) { c.gravityScale = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("AngularDamping", "How quickly the body's spin slows down", "number",
			([](Self& c) { return c.angularDamping; }),
			([](Self& c, float v) { c.angularDamping = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("LinearDamping", "How quickly the body's movement slows down", "number",
			([](Self& c) { return c.linearDamping; }),
			([](Self& c, float v) { c.linearDamping = v; }))
		REFLECT_LUA_FUNCTION(ApplyImpulse, "Apply an instant change in momentum at the body's centre")
		REFLECT_LUA_FUNCTION(ApplyImpulseAtPoint, "Apply an instant change in momentum at a world-space point")
		REFLECT_LUA_FUNCTION(ApplyForce, "Apply a force at the body's centre for this physics step")
		REFLECT_LUA_FUNCTION(ApplyForceAtPoint, "Apply a force at a world-space point for this physics step")
		REFLECT_LUA_FUNCTION(ApplyTorque, "Apply a rotational force for this physics step")
		REFLECT_LUA_FUNCTION(GetLinearVelocity, "Get the body's velocity")
		REFLECT_LUA_FUNCTION(SetLinearVelocity, "Set the body's velocity")
		REFLECT_LUA_FUNCTION(GetAngularVelocity, "Get the body's spin speed, in radians per second")
		REFLECT_LUA_FUNCTION(SetAngularVelocity, "Set the body's spin speed, in radians per second")
		REFLECT_LUA_FUNCTION(SetTransform, "Teleport the body to a position and angle (radians)")
		REFLECT_LUA_FUNCTION_CUSTOM("GetTransform", "Get the body's position and angle (radians) as two return values", [](Self& c) {
			Vector2f position;
			float rotation;
			c.GetTransform(position, rotation);
			return std::make_tuple(position, rotation);
		})
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(type, fixedRotation, gravityScale, angularDamping, linearDamping);
	}
};