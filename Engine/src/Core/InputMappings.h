#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace tinyxml2
{
class XMLDocument;
}

enum class InputActionValueType
{
	Digital,
	Axis1D,
	Axis2D
};

enum class InputBindingDevice
{
	Key,
	MouseButton,
	GamepadButton,
	GamepadAxis,
	MouseAxis
};

// Restricts when a binding contributes to its action's value:
// Down fires every frame the underlying input is held, Pressed/Released
// pulse for a single frame on the respective edge of the underlying input.
enum class InputTrigger
{
	Down,
	Pressed,
	Released
};
enum class InputAxisComponent
{
	X,
	Y
};

const char* ToString(InputActionValueType type);
InputActionValueType InputActionValueTypeFromString(const std::string& str);
const char* ToString(InputBindingDevice device);
InputBindingDevice InputBindingDeviceFromString(const std::string& str);
const char* ToString(InputTrigger trigger);
InputTrigger InputTriggerFromString(const std::string& str);
const char* ToString(InputAxisComponent component);
InputAxisComponent InputAxisComponentFromString(const std::string& str);

struct InputBinding
{
	InputBindingDevice device = InputBindingDevice::Key;
	int code = 0;
	InputTrigger trigger = InputTrigger::Down;
	bool negate = false;
	float scale = 1.0f;
	float deadzone = 0.0f;
	InputAxisComponent axisComponent = InputAxisComponent::X;
};

struct InputActionDefinition
{
	std::string name;
	InputActionValueType valueType = InputActionValueType::Digital;
	std::vector<InputBinding> bindings;
};

namespace InputMappings
{
	extern const char* FilePath;

	bool Load(std::vector<InputActionDefinition>& actions, const std::filesystem::path& filepath);
	bool LoadFromData(std::vector<InputActionDefinition>& actions, const std::vector<uint8_t>& data);
	bool Save(const std::vector<InputActionDefinition>& actions, const std::filesystem::path& filepath);
}
