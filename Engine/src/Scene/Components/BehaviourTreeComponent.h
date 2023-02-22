#pragma once

#include "cereal/access.hpp"

#include "AI/BehaviorTree.h"

struct BehaviourTreeComponent
{
	BehaviourTreeComponent() = default;
	BehaviourTreeComponent(const BehaviourTreeComponent&) = default;
	BehaviourTree::BehaviourTree behaviourTree;

	std::filesystem::path filepath;

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
