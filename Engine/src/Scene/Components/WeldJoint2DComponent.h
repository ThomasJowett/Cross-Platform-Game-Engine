#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

class b2Body;
class b2WeldJoint;

struct WeldJoint2DComponent
{
	bool collideConnected = false;
	Vector2f anchor;
	float damping = 0.0f;
	float stiffness = 0.0f;

	entt::entity entityA;
	entt::entity entityB;

	b2Body* bodyA;
	b2Body* bodyB;

	b2WeldJoint* joint;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(anchor, collideConnected, damping, stiffness);
	}
};
