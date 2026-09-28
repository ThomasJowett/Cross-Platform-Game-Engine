#include "InputActionSystem.h"
#include "Core/Input.h"

#include <algorithm>
#include <cmath>

const char* ToString(InputActionPhase phase)
{
	switch (phase)
	{
	case InputActionPhase::Started:   return "Started";
	case InputActionPhase::Triggered: return "Triggered";
	case InputActionPhase::Completed: return "Completed";
	}
	return "Triggered";
}

std::vector<InputActionDefinition> InputActionSystem::s_Actions;
std::unordered_map<std::string, InputActionSystem::ActionRuntimeState> InputActionSystem::s_ActionStates;
std::unordered_map<const InputBinding*, bool> InputActionSystem::s_BindingWasActive;
std::vector<InputActionEvent> InputActionSystem::s_PendingEvents;

void InputActionSystem::SetActions(std::vector<InputActionDefinition> actions)
{
	s_Actions = std::move(actions);
	ResetState();
}

void InputActionSystem::LoadMappings(const std::filesystem::path& filepath)
{
	std::vector<InputActionDefinition> actions;
	InputMappings::Load(actions, filepath);
	SetActions(std::move(actions));
}

void InputActionSystem::LoadMappingsFromData(const std::vector<uint8_t>& data)
{
	std::vector<InputActionDefinition> actions;
	InputMappings::LoadFromData(actions, data);
	SetActions(std::move(actions));
}

void InputActionSystem::ResetState()
{
	s_ActionStates.clear();
	s_BindingWasActive.clear();
	s_PendingEvents.clear();
}

float InputActionSystem::EvaluateBinding(const InputBinding& binding)
{
	float raw = 0.0f;

	switch (binding.device)
	{
	case InputBindingDevice::Key:
		raw = Input::IsKeyPressed(binding.code) ? 1.0f : 0.0f;
		break;
	case InputBindingDevice::MouseButton:
		raw = Input::IsMouseButtonPressed(binding.code) ? 1.0f : 0.0f;
		break;
	case InputBindingDevice::GamepadButton:
		// v1 assumes a single local player, so this always polls joystick slot 0.
		raw = Input::IsJoystickButtonPressed(0, binding.code) ? 1.0f : 0.0f;
		break;
	case InputBindingDevice::GamepadAxis:
		raw = (float)Input::GetJoystickAxis(0, binding.code);
		break;
	case InputBindingDevice::MouseAxis:
	{
		Vector2f delta = Input::GetMouseDelta();
		raw = binding.code == 0 ? delta.x : delta.y;
		break;
	}
	}

	if (binding.deadzone > 0.0f && std::fabs(raw) < binding.deadzone)
		raw = 0.0f;

	bool rawActive = raw != 0.0f;
	bool& wasActive = s_BindingWasActive[&binding];

	float sample = 0.0f;
	switch (binding.trigger)
	{
	case InputTrigger::Down:
		sample = raw;
		break;
	case InputTrigger::Pressed:
		sample = (rawActive && !wasActive) ? 1.0f : 0.0f;
		break;
	case InputTrigger::Released:
		sample = (!rawActive && wasActive) ? 1.0f : 0.0f;
		break;
	}
	wasActive = rawActive;

	if (binding.negate)
		sample = -sample;

	return sample * binding.scale;
}

void InputActionSystem::Update()
{
	s_PendingEvents.clear();

	for (const InputActionDefinition& action : s_Actions)
	{
		Vector2f accumulated;
		for (const InputBinding& binding : action.bindings)
		{
			float sample = EvaluateBinding(binding);

			if (binding.axisComponent == InputAxisComponent::Y)
				accumulated.y += sample;
			else
				accumulated.x += sample;
		}

		Vector2f value(std::clamp(accumulated.x, -1.0f, 1.0f), std::clamp(accumulated.y, -1.0f, 1.0f));

		ActionRuntimeState& state = s_ActionStates[action.name];
		bool isActive = value.x != 0.0f || value.y != 0.0f;

		if (isActive && !state.wasActive)
			s_PendingEvents.push_back({ action.name, ToString(InputActionPhase::Started) });
		if (isActive)
			s_PendingEvents.push_back({ action.name, ToString(InputActionPhase::Triggered) });
		if (!isActive && state.wasActive)
			s_PendingEvents.push_back({ action.name, ToString(InputActionPhase::Completed) });

		state.value = value;
		state.wasActive = isActive;
	}
}

InputActionValue InputActionSystem::GetActionValue(const std::string& actionName)
{
	auto it = s_ActionStates.find(actionName);
	if (it == s_ActionStates.end())
		return {};

	return { it->second.value };
}

bool InputActionSystem::IsActionTriggered(const std::string& actionName)
{
	return GetActionValue(actionName).IsActive();
}
