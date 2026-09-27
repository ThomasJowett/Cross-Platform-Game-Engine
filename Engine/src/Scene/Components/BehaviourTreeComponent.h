#pragma once

#include "cereal/access.hpp"

#include "AI/BehaviourTree.h"
#include "AI/BehaviourTreeSerializer.h"
#include "Scripting/Lua/LuaBindings.h"

struct BehaviourTreeComponent
{
	BehaviourTreeComponent() = default;
	// Copies reload the tree so each entity gets its own node state and blackboard
	BehaviourTreeComponent(const BehaviourTreeComponent& other)
		:filepath(other.filepath)
	{
		behaviourTree = filepath.empty() ? other.behaviourTree : BehaviourTree::Serializer::Load(filepath);
	}
	BehaviourTreeComponent& operator=(const BehaviourTreeComponent& other)
	{
		if (this != &other)
		{
			BehaviourTreeComponent copy(other);
			*this = std::move(copy);
		}
		return *this;
	}
	BehaviourTreeComponent(BehaviourTreeComponent&&) = default;
	BehaviourTreeComponent& operator=(BehaviourTreeComponent&&) = default;

	// Loaded per entity rather than shared through AssetManager, as the tree holds runtime state
	Ref<BehaviourTree::BehaviourTree> behaviourTree;

	std::filesystem::path filepath;

	Ref<BehaviourTree::Blackboard> GetBlackboard() const { return behaviourTree ? behaviourTree->getBlackboard() : nullptr; }

	REFLECT_LUA_BEGIN(BehaviourTreeComponent)
		REFLECT_LUA_FUNCTION(GetBlackboard, "Get the blackboard shared by this entity's behaviour tree and its custom tasks, or nil if no tree is loaded")
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(filepath.string());
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(relativePath);

		if (!relativePath.empty())
		{
			filepath = relativePath;
			behaviourTree = BehaviourTree::Serializer::Load(filepath);
		}
	}
};
