#pragma once

#include "cereal/access.hpp"

struct StateMachineComponent
{
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
