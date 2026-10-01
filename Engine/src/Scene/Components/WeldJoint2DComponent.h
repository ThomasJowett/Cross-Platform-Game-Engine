#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Scripting/Lua/LuaBindings.h"

class b2Body;
class b2WeldJoint;

struct WeldJoint2DComponent
{
	bool collideConnected = false;
	float damping = 0.0f;
	float stiffness = 0.0f;

	entt::entity entityA;
	entt::entity entityB;

	b2Body* bodyA;
	b2Body* bodyB;

	b2WeldJoint* joint;
	REFLECT_LUA_BEGIN(WeldJoint2DComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("CollideConnected", "Whether the two welded bodies can still collide with each other", "boolean",
			([](Self& c) { return c.collideConnected; }),
			([](Self& c, bool v) { c.collideConnected = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Damping", "Damping of the joint's spring, if Stiffness is above 0", "number",
			([](Self& c) { return c.damping; }),
			([](Self& c, float v) { c.damping = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Stiffness", "Spring stiffness of the weld; 0 makes it rigid", "number",
			([](Self& c) { return c.stiffness; }),
			([](Self& c, float v) { c.stiffness = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(collideConnected, damping, stiffness);
	}
};
