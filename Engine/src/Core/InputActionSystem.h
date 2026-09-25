#pragma once

#include "Core/core.h"
#include "Core/InputMappings.h"
#include "math/Vector2f.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

enum class InputActionPhase
{
	Started,   // The action's value went from inactive to active this frame
	Triggered, // The action's value is active this frame (fires every frame it stays active)
	Completed  // The action's value went from active to inactive this frame
};

const char* ToString(InputActionPhase phase);

struct InputActionValue
{
	Vector2f value;

	bool IsActive() const { return value.x != 0.0f || value.y != 0.0f; }
};

struct InputActionEvent
{
	std::string actionName;
	std::string phase;
};
class InputActionSystem
{
public:
	static void SetActions(std::vector<InputActionDefinition> actions);
	static void LoadMappings(const std::filesystem::path& filepath);
	static void LoadMappingsFromData(const std::vector<uint8_t>& data);

	static const std::vector<InputActionDefinition>& GetActions() { return s_Actions; }

	static void ResetState();

	static void Update();

	static InputActionValue GetActionValue(const std::string& actionName);
	static bool IsActionTriggered(const std::string& actionName);

	static const std::vector<InputActionEvent>& GetPendingEvents() { return s_PendingEvents; }

private:
	struct ActionRuntimeState
	{
		Vector2f value;
		bool wasActive = false;
	};

	static float EvaluateBinding(const InputBinding& binding);

	static std::vector<InputActionDefinition> s_Actions;
	static std::unordered_map<std::string, ActionRuntimeState> s_ActionStates;
	static std::unordered_map<const InputBinding*, bool> s_BindingWasActive;
	static std::vector<InputActionEvent> s_PendingEvents;
};
