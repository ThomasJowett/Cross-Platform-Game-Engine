#pragma once

#include "Core/core.h"

#include <filesystem>
#include <vector>

namespace tinyxml2
{
class XMLDocument;
}

class StateMachine;

class StateMachineSerializer
{
public:
	StateMachineSerializer() = delete;
	static bool Serialize(const std::filesystem::path& filepath, StateMachine* stateMachine);
	// Loads a new state machine instance, from the asset bundle in exported games or from disk otherwise
	static Ref<StateMachine> Load(const std::filesystem::path& filepath);
	static Ref<StateMachine> Deserialize(const std::filesystem::path& filepath);
	static Ref<StateMachine> Deserialize(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);

private:
	static Ref<StateMachine> LoadXML(tinyxml2::XMLDocument* doc);
};
