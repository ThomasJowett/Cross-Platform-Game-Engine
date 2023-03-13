#pragma once

#include "Core/core.h"

#include <filesystem>

namespace tinyxml2
{
class XMLElement;
}

namespace BehaviourTree
{
class BehaviourTree;
class Node;
class Serializer
{
public:
	Serializer() = delete;
	static bool Serialize(const std::filesystem::path& filepath, BehaviourTree* behaviourTree);
	static Ref<BehaviourTree> Deserialize(const std::filesystem::path& filepath);
	static void SerializeNode(tinyxml2::XMLElement* pElement, const Ref<Node> node);
	static Ref<Node> DeserializeNode(tinyxml2::XMLElement* pElement, BehaviourTree* behaviourTree);
};
}
