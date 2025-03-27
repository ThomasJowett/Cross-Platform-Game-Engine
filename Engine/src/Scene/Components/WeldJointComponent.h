#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

class b2Body;
class b2WeldJoint;

struct WeldJointComponent
{
	Vector2f offset;

	b2Body* bodyA;
	b2Body* bodyB;

	b2WeldJoint* joint;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(offset);
	}
};
