#include "InputMappingsPanel.h"
#include "IconsFontAwesome6.h"
#include "MainDockSpace.h"
#include "ImGui/ImGuiUtilities.h"
#include "Core/Input.h"
#include "Core/Joysticks.h"
#include "Core/Application.h"
#include "Core/InputActionSystem.h"

#include "imgui/imgui.h"

#include <cstring>

namespace
{
	std::string GetKeyDisplayName(int keycode)
	{
		if (keycode >= KEY_A && keycode <= KEY_Z)
			return std::string(1, (char)('A' + (keycode - KEY_A)));
		if (keycode >= KEY_0 && keycode <= KEY_9)
			return std::string(1, (char)('0' + (keycode - KEY_0)));
		if (keycode >= KEY_F1 && keycode <= KEY_F25)
			return "F" + std::to_string(keycode - KEY_F1 + 1);
		if (keycode >= KEY_KP_0 && keycode <= KEY_KP_9)
			return "Numpad " + std::to_string(keycode - KEY_KP_0);

		switch (keycode)
		{
		case KEY_SPACE: return "Space";
		case KEY_APOSTROPHE: return "'";
		case KEY_COMMA: return ",";
		case KEY_MINUS: return "-";
		case KEY_PERIOD: return ".";
		case KEY_SLASH: return "/";
		case KEY_SEMICOLON: return ";";
		case KEY_EQUAL: return "=";
		case KEY_LEFT_BRACKET: return "[";
		case KEY_BACKSLASH: return "\\";
		case KEY_RIGHT_BRACKET: return "]";
		case KEY_GRAVE_ACCENT: return "`";
		case KEY_WORLD_1: return "World 1";
		case KEY_WORLD_2: return "World 2";
		case KEY_ESCAPE: return "Escape";
		case KEY_ENTER: return "Enter";
		case KEY_TAB: return "Tab";
		case KEY_BACKSPACE: return "Backspace";
		case KEY_INSERT: return "Insert";
		case KEY_DELETE: return "Delete";
		case KEY_RIGHT: return "Right Arrow";
		case KEY_LEFT: return "Left Arrow";
		case KEY_DOWN: return "Down Arrow";
		case KEY_UP: return "Up Arrow";
		case KEY_PAGE_UP: return "Page Up";
		case KEY_PAGE_DOWN: return "Page Down";
		case KEY_HOME: return "Home";
		case KEY_END: return "End";
		case KEY_CAPS_LOCK: return "Caps Lock";
		case KEY_SCROLL_LOCK: return "Scroll Lock";
		case KEY_NUM_LOCK: return "Num Lock";
		case KEY_PRINT_SCREEN: return "Print Screen";
		case KEY_PAUSE: return "Pause";
		case KEY_KP_DECIMAL: return "Numpad .";
		case KEY_KP_DIVIDE: return "Numpad /";
		case KEY_KP_MULTIPLY: return "Numpad *";
		case KEY_KP_SUBTRACT: return "Numpad -";
		case KEY_KP_ADD: return "Numpad +";
		case KEY_KP_ENTER: return "Numpad Enter";
		case KEY_KP_EQUAL: return "Numpad =";
		case KEY_LEFT_SHIFT: return "Left Shift";
		case KEY_LEFT_CONTROL: return "Left Ctrl";
		case KEY_LEFT_ALT: return "Left Alt";
		case KEY_LEFT_SUPER: return "Left Super";
		case KEY_RIGHT_SHIFT: return "Right Shift";
		case KEY_RIGHT_CONTROL: return "Right Ctrl";
		case KEY_RIGHT_ALT: return "Right Alt";
		case KEY_RIGHT_SUPER: return "Right Super";
		case KEY_MENU: return "Menu";
		default: return "Key " + std::to_string(keycode);
		}
	}

	std::string GetMouseButtonDisplayName(int code)
	{
		switch (code)
		{
		case MOUSE_BUTTON_LEFT: return "Left Click";
		case MOUSE_BUTTON_RIGHT: return "Right Click";
		case MOUSE_BUTTON_MIDDLE: return "Middle Click";
		default: return "Mouse " + std::to_string(code + 1);
		}
	}

	struct NamedCode { const char* name; int code; };

	const NamedCode s_GamepadButtons[] = {
		{ "A", GAMEPAD_BUTTON_A },
		{ "B", GAMEPAD_BUTTON_B },
		{ "X", GAMEPAD_BUTTON_X },
		{ "Y", GAMEPAD_BUTTON_Y },
		{ "Left Bumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "Right Bumper", GAMEPAD_BUTTON_RIGHT_BUMPER },
		{ "Back", GAMEPAD_BUTTON_BACK },
		{ "Start", GAMEPAD_BUTTON_START },
		{ "Guide", GAMEPAD_BUTTON_GUIDE },
		{ "Left Stick Click", GAMEPAD_BUTTON_LEFT_THUMB },
		{ "Right Stick Click", GAMEPAD_BUTTON_RIGHT_THUMB },
		{ "D-Pad Up", GAMEPAD_BUTTON_DPAD_UP },
		{ "D-Pad Right", GAMEPAD_BUTTON_DPAD_RIGHT },
		{ "D-Pad Down", GAMEPAD_BUTTON_DPAD_DOWN },
		{ "D-Pad Left", GAMEPAD_BUTTON_DPAD_LEFT },
	};

	const NamedCode s_GamepadAxes[] = {
		{ "Left Stick X", GAMEPAD_AXIS_LEFT_X },
		{ "Left Stick Y", GAMEPAD_AXIS_LEFT_Y },
		{ "Right Stick X", GAMEPAD_AXIS_RIGHT_X },
		{ "Right Stick Y", GAMEPAD_AXIS_RIGHT_Y },
		{ "Left Trigger", GAMEPAD_AXIS_LEFT_TRIGGER },
		{ "Right Trigger", GAMEPAD_AXIS_RIGHT_TRIGGER },
	};

	const NamedCode s_MouseAxes[] = {
		{ "X", 0 },
		{ "Y", 1 },
	};

	const char* FindName(const NamedCode* table, size_t count, int code)
	{
		for (size_t i = 0; i < count; i++)
			if (table[i].code == code)
				return table[i].name;
		return "Unknown";
	}

	// Combo listing every entry in a NamedCode table; returns true and writes the selected
	// code into `code` when the user picks a different entry.
	template<size_t N>
	bool NamedCodeCombo(const char* label, int& code, const NamedCode(&table)[N])
	{
		bool changed = false;
		if (ImGui::BeginCombo(label, FindName(table, N, code)))
		{
			for (const NamedCode& entry : table)
			{
				if (ImGui::Selectable(entry.name, entry.code == code))
				{
					code = entry.code;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}
}

InputMappingsPanel::InputMappingsPanel(bool* show)
	:m_Show(show), Layer("Input Mappings Panel")
{
}

void InputMappingsPanel::OnAttach()
{
	ReadMappingsFile();
}

void InputMappingsPanel::OnDetach()
{
	SaveMappingsFile();
}

void InputMappingsPanel::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<AppOpenDocumentChangedEvent>(BIND_EVENT_FN(InputMappingsPanel::OnOpenDocumentChanged));
}

void InputMappingsPanel::ReadMappingsFile()
{
	m_Actions.clear();

	if (Application::GetOpenDocumentDirectory().empty())
		return;

	InputMappings::Load(m_Actions, Application::GetOpenDocumentDirectory() / InputMappings::FilePath);
}

void InputMappingsPanel::SaveMappingsFile()
{
	if (Application::GetOpenDocumentDirectory().empty())
		return;

	std::filesystem::path filepath = Application::GetOpenDocumentDirectory() / InputMappings::FilePath;

	std::error_code errorCode;
	std::filesystem::create_directories(filepath.parent_path(), errorCode);
	if (errorCode)
	{
		ENGINE_ERROR("Could not create directory for input mappings file: {0}, {1}", filepath.parent_path().string(), errorCode.message());
		return;
	}

	InputMappings::Save(m_Actions, filepath);
	InputActionSystem::SetActions(m_Actions);
	m_Dirty = false;
}

bool InputMappingsPanel::PollForCapture(InputBinding& binding)
{
	bool anyPressed = false;
	InputBindingDevice pressedDevice = InputBindingDevice::Key;
	int pressedCode = -1;

	for (int key = KEY_SPACE; key <= KEY_LAST; ++key)
	{
		if (Input::IsKeyPressed(key))
		{
			anyPressed = true;
			if (pressedCode < 0) { pressedDevice = InputBindingDevice::Key; pressedCode = key; }
		}
	}
	for (int button = MOUSE_BUTTON_1; button <= MOUSE_BUTTON_LAST; ++button)
	{
		if (Input::IsMouseButtonPressed(button))
		{
			anyPressed = true;
			if (pressedCode < 0) { pressedDevice = InputBindingDevice::MouseButton; pressedCode = button; }
		}
	}

	if (!anyPressed)
	{
		m_ReadyToCapture = true;
		return false;
	}

	if (m_ReadyToCapture && pressedCode >= 0)
	{
		binding.device = pressedDevice;
		binding.code = pressedCode;
		return true;
	}
	return false;
}

void InputMappingsPanel::DrawBinding(InputBinding& binding, InputActionValueType valueType, int actionIndex, int bindingIndex)
{
	ImGui::PushID(bindingIndex);

	bool listening = m_ListeningActionIndex == actionIndex && m_ListeningBindingIndex == bindingIndex;

	ImGui::SetNextItemWidth(140);
	if (ImGui::BeginCombo("##Device", ToString(binding.device)))
	{
		for (InputBindingDevice candidate : { InputBindingDevice::Key, InputBindingDevice::MouseButton, InputBindingDevice::GamepadButton, InputBindingDevice::GamepadAxis, InputBindingDevice::MouseAxis })
		{
			if (ImGui::Selectable(ToString(candidate), candidate == binding.device))
			{
				binding.device = candidate;
				m_Dirty = true;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	bool axisDevice = binding.device == InputBindingDevice::GamepadAxis || binding.device == InputBindingDevice::MouseAxis;
	switch (binding.device)
	{
	case InputBindingDevice::GamepadButton:
		ImGui::SetNextItemWidth(150);
		if (NamedCodeCombo("##Code", binding.code, s_GamepadButtons))
			m_Dirty = true;
		break;
	case InputBindingDevice::GamepadAxis:
		ImGui::SetNextItemWidth(150);
		if (NamedCodeCombo("##Code", binding.code, s_GamepadAxes))
			m_Dirty = true;
		break;
	case InputBindingDevice::MouseAxis:
		ImGui::SetNextItemWidth(150);
		if (NamedCodeCombo("##Code", binding.code, s_MouseAxes))
			m_Dirty = true;
		break;
	case InputBindingDevice::Key:
	case InputBindingDevice::MouseButton:
	default:
	{
		std::string codeName = binding.device == InputBindingDevice::MouseButton ? GetMouseButtonDisplayName(binding.code) : GetKeyDisplayName(binding.code);
		ImGui::SetNextItemWidth(150);
		if (ImGui::Button(listening ? "Listening..." : codeName.c_str(), ImVec2(150, 0)))
		{
			m_ListeningActionIndex = actionIndex;
			m_ListeningBindingIndex = bindingIndex;
			m_ReadyToCapture = false;
		}
		ImGui::Tooltip("Click, then press the key/mouse button to bind");
		break;
	}
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(110);
	if (ImGui::BeginCombo("##Trigger", ToString(binding.trigger)))
	{
		for (InputTrigger candidate : { InputTrigger::Down, InputTrigger::Pressed, InputTrigger::Released })
		{
			if (ImGui::Selectable(ToString(candidate), candidate == binding.trigger))
			{
				binding.trigger = candidate;
				m_Dirty = true;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("Negate", &binding.negate))
		m_Dirty = true;

	// Scale/Axis only affect an action's numeric value - meaningless for a Digital action,
	// which just reads as active/inactive regardless of magnitude or which component it's in.
	if (valueType != InputActionValueType::Digital)
	{
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("Scale", &binding.scale, 0.01f))
			m_Dirty = true;
	}

	if (axisDevice)
	{
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("Deadzone", &binding.deadzone, 0.01f, 0.0f, 1.0f))
			m_Dirty = true;
	}

	if (valueType == InputActionValueType::Axis2D)
	{
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::BeginCombo("Axis", ToString(binding.axisComponent)))
		{
			for (InputAxisComponent candidate : { InputAxisComponent::X, InputAxisComponent::Y })
			{
				if (ImGui::Selectable(ToString(candidate), candidate == binding.axisComponent))
				{
					binding.axisComponent = candidate;
					m_Dirty = true;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Tooltip("Which component of the Axis2D action this binding feeds (e.g. W -> +Y)");
	}

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_TRASH_CAN"##RemoveBinding"))
		m_PendingRemoveBinding = { actionIndex, bindingIndex };

	if (listening)
	{
		if (PollForCapture(binding))
		{
			m_Dirty = true;
			m_ListeningActionIndex = -1;
			m_ListeningBindingIndex = -1;
		}
	}

	ImGui::PopID();
}

void InputMappingsPanel::DrawAction(InputActionDefinition& action, int actionIndex)
{
	ImGui::PushID(actionIndex);

	char nameBuffer[128];
	strncpy(nameBuffer, action.name.c_str(), sizeof(nameBuffer) - 1);
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';

	ImGui::SetNextItemWidth(160);
	if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
	{
		action.name = nameBuffer;
		m_Dirty = true;
	}
	ImGui::Tooltip("Referenced from Lua as InputAction.<Name>");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::BeginCombo("##ValueType", ToString(action.valueType)))
	{
		for (InputActionValueType candidate : { InputActionValueType::Digital, InputActionValueType::Axis1D, InputActionValueType::Axis2D })
		{
			if (ImGui::Selectable(ToString(candidate), candidate == action.valueType))
			{
				action.valueType = candidate;
				m_Dirty = true;
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_TRASH_CAN" Remove Action"))
		m_PendingRemoveAction = actionIndex;

	ImGui::Indent();
	for (int bindingIndex = 0; bindingIndex < (int)action.bindings.size(); ++bindingIndex)
		DrawBinding(action.bindings[bindingIndex], action.valueType, actionIndex, bindingIndex);

	if (ImGui::Button(ICON_FA_PLUS" Add Binding"))
	{
		action.bindings.push_back(InputBinding());
		m_Dirty = true;
	}
	ImGui::Unindent();

	ImGui::PopID();
}

void InputMappingsPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		m_WasShown = false;
		return;
	}

	if (!m_WasShown)
	{
		ReadMappingsFile();
		m_WasShown = true;
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

	ImGui::SetNextWindowSize(ImVec2(750, 450), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_SLIDERS" Input Mappings", m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save"))
					SaveMappingsFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_PendingRemoveBinding = { -1, -1 };
		m_PendingRemoveAction = -1;

		for (int actionIndex = 0; actionIndex < (int)m_Actions.size(); ++actionIndex)
		{
			ImGui::Separator();
			DrawAction(m_Actions[actionIndex], actionIndex);
		}
		ImGui::Separator();

		if (ImGui::Button(ICON_FA_PLUS" Add Action"))
		{
			InputActionDefinition action;
			action.name = "NewAction";
			m_Actions.push_back(std::move(action));
			m_Dirty = true;
		}

		if (m_PendingRemoveBinding.first >= 0)
		{
			auto& bindings = m_Actions[m_PendingRemoveBinding.first].bindings;
			bindings.erase(bindings.begin() + m_PendingRemoveBinding.second);
			m_Dirty = true;
		}
		if (m_PendingRemoveAction >= 0)
		{
			m_Actions.erase(m_Actions.begin() + m_PendingRemoveAction);
			m_Dirty = true;
		}
	}
	ImGui::End();
}
