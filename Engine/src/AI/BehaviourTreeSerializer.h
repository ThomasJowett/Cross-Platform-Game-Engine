#pragma once

#include "AI/BehaviorTree.h"

#include <filesystem>

namespace BehaviourTree
{
	class Serializer
	{
	public:
		Serializer(BehaviourTree* behaviourTree);

		bool Serialize(const std::filesystem::path& filepath) const;
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		BehaviourTree* m_BehaviourTree;
	};
}