#pragma once

#include "AI/BehaviorTree.h"

#include <filesystem>

namespace tinyxml2
{
class XMLElement;
}

namespace BehaviourTree
{
	class Serializer
	{
	public:
		Serializer(BehaviourTree* behaviourTree);

		bool Serialize(const std::filesystem::path& filepath) const;
		bool Deserialize(const std::filesystem::path& filepath);

		void SerializeNode(tinyxml2::XMLElement* pElement, const Ref<Node> node) const;
		Ref<Node> DeserializeNode(tinyxml2::XMLElement* pElement);
	private:
		BehaviourTree* m_BehaviourTree;
	};
}