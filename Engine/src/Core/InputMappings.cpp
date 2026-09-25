#include "InputMappings.h"

#include "TinyXml2/tinyxml2.h"
#include "Logging/Logger.h"

const char* ToString(InputActionValueType type)
{
	switch (type)
	{
	case InputActionValueType::Digital: return "Digital";
	case InputActionValueType::Axis1D:  return "Axis1D";
	case InputActionValueType::Axis2D:  return "Axis2D";
	}
	return "Digital";
}

InputActionValueType InputActionValueTypeFromString(const std::string& str)
{
	if (str == "Axis1D") return InputActionValueType::Axis1D;
	if (str == "Axis2D") return InputActionValueType::Axis2D;
	return InputActionValueType::Digital;
}

const char* ToString(InputBindingDevice device)
{
	switch (device)
	{
	case InputBindingDevice::Key:           return "Key";
	case InputBindingDevice::MouseButton:   return "MouseButton";
	case InputBindingDevice::GamepadButton: return "GamepadButton";
	case InputBindingDevice::GamepadAxis:   return "GamepadAxis";
	case InputBindingDevice::MouseAxis:     return "MouseAxis";
	}
	return "Key";
}

InputBindingDevice InputBindingDeviceFromString(const std::string& str)
{
	if (str == "MouseButton")   return InputBindingDevice::MouseButton;
	if (str == "GamepadButton") return InputBindingDevice::GamepadButton;
	if (str == "GamepadAxis")   return InputBindingDevice::GamepadAxis;
	if (str == "MouseAxis")     return InputBindingDevice::MouseAxis;
	return InputBindingDevice::Key;
}

const char* ToString(InputTrigger trigger)
{
	switch (trigger)
	{
	case InputTrigger::Down:     return "Down";
	case InputTrigger::Pressed:  return "Pressed";
	case InputTrigger::Released: return "Released";
	}
	return "Down";
}

InputTrigger InputTriggerFromString(const std::string& str)
{
	if (str == "Pressed")  return InputTrigger::Pressed;
	if (str == "Released") return InputTrigger::Released;
	return InputTrigger::Down;
}

const char* ToString(InputAxisComponent component)
{
	switch (component)
	{
	case InputAxisComponent::X: return "X";
	case InputAxisComponent::Y: return "Y";
	}
	return "X";
}

InputAxisComponent InputAxisComponentFromString(const std::string& str)
{
	if (str == "Y") return InputAxisComponent::Y;
	return InputAxisComponent::X;
}

namespace InputMappings
{
	const char* FilePath = "Generated/InputMappings.inputmappings";

	static void WriteBinding(tinyxml2::XMLElement* pParent, const InputBinding& binding)
	{
		tinyxml2::XMLElement* pBinding = pParent->InsertNewChildElement("Binding");
		pBinding->SetAttribute("Device", ToString(binding.device));
		pBinding->SetAttribute("Code", binding.code);
		pBinding->SetAttribute("Trigger", ToString(binding.trigger));
		pBinding->SetAttribute("Negate", binding.negate);
		pBinding->SetAttribute("Scale", binding.scale);
		pBinding->SetAttribute("Deadzone", binding.deadzone);
		pBinding->SetAttribute("AxisComponent", ToString(binding.axisComponent));
	}

	static InputBinding ReadBinding(const tinyxml2::XMLElement* pElement)
	{
		InputBinding binding;
		binding.device = InputBindingDeviceFromString(pElement->Attribute("Device") ? pElement->Attribute("Device") : "Key");
		binding.code = pElement->IntAttribute("Code", 0);
		binding.trigger = InputTriggerFromString(pElement->Attribute("Trigger") ? pElement->Attribute("Trigger") : "Down");
		binding.negate = pElement->BoolAttribute("Negate", false);
		binding.scale = pElement->FloatAttribute("Scale", 1.0f);
		binding.deadzone = pElement->FloatAttribute("Deadzone", 0.0f);
		binding.axisComponent = InputAxisComponentFromString(pElement->Attribute("AxisComponent") ? pElement->Attribute("AxisComponent") : "X");
		return binding;
	}

	static bool LoadXML(std::vector<InputActionDefinition>& actions, tinyxml2::XMLDocument& doc)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("InputMappings");
		if (!pRoot)
		{
			ENGINE_ERROR("Could not read input mappings file, no InputMappings node");
			return false;
		}

		actions.clear();

		for (tinyxml2::XMLElement* pAction = pRoot->FirstChildElement("Action"); pAction; pAction = pAction->NextSiblingElement("Action"))
		{
			InputActionDefinition action;
			action.name = pAction->Attribute("Name") ? pAction->Attribute("Name") : "";
			action.valueType = InputActionValueTypeFromString(pAction->Attribute("ValueType") ? pAction->Attribute("ValueType") : "Digital");

			for (tinyxml2::XMLElement* pBinding = pAction->FirstChildElement("Binding"); pBinding; pBinding = pBinding->NextSiblingElement("Binding"))
				action.bindings.push_back(ReadBinding(pBinding));

			actions.push_back(std::move(action));
		}

		return true;
	}

	bool Load(std::vector<InputActionDefinition>& actions, const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
			return false;

		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(filepath.string().c_str()) != tinyxml2::XML_SUCCESS)
		{
			ENGINE_ERROR("Could not load input mappings file: {0}, {1} on line {2}", filepath.string(), doc.ErrorName(), doc.ErrorLineNum());
			return false;
		}

		return LoadXML(actions, doc);
	}

	bool LoadFromData(std::vector<InputActionDefinition>& actions, const std::vector<uint8_t>& data)
	{
		tinyxml2::XMLDocument doc;
		if (doc.Parse((const char*)data.data(), data.size()) != tinyxml2::XML_SUCCESS)
		{
			ENGINE_ERROR("Could not parse input mappings data: {0} on line {1}", doc.ErrorName(), doc.ErrorLineNum());
			return false;
		}

		return LoadXML(actions, doc);
	}

	bool Save(const std::vector<InputActionDefinition>& actions, const std::filesystem::path& filepath)
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* pRoot = doc.NewElement("InputMappings");
		doc.InsertFirstChild(pRoot);

		for (const InputActionDefinition& action : actions)
		{
			tinyxml2::XMLElement* pAction = pRoot->InsertNewChildElement("Action");
			pAction->SetAttribute("Name", action.name.c_str());
			pAction->SetAttribute("ValueType", ToString(action.valueType));

			for (const InputBinding& binding : action.bindings)
				WriteBinding(pAction, binding);
		}

		return doc.SaveFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS;
	}
}
