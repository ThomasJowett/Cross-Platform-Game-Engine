#pragma once

#include "cereal/access.hpp"

#include "AI/BehaviourTree.h"
#include "AI/BehaviourTreeSerializer.h"

struct BehaviourTreeComponent
{
	BehaviourTreeComponent() = default;
	BehaviourTreeComponent(const BehaviourTreeComponent&) = default;
	Ref<BehaviourTree::BehaviourTree> behaviourTree;

	std::filesystem::path filepath;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath;
		if (!filepath.empty())
		{
			relativePath = FileUtils::RelativePath(filepath, Application::GetOpenDocumentDirectory()).string();
		}
		archive(relativePath);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(relativePath);

		if (!relativePath.empty())
		{
			filepath = std::filesystem::absolute((Application::GetOpenDocumentDirectory() / relativePath));
			behaviourTree = BehaviourTree::Serializer::Deserialize(filepath);
		}
	}
};
