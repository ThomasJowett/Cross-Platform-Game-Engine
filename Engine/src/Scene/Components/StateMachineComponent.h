#pragma once

#include "cereal/access.hpp"

#include "AI/StateMachine.h"

struct StateMachineComponent
{
	StateMachineComponent() = default;
	StateMachineComponent(const StateMachineComponent&) = default;

	StateMachine stateMachine;
private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{

	}

	template<typename Archive>
	void load(Archive& archive)
	{

	}
};
