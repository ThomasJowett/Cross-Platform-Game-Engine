#pragma once

#include <filesystem>
#include "Core/Layer.h"
#include "Core/InputMappings.h"
class InputMappingsPanel
	:public Layer
{
public:
	explicit InputMappingsPanel(bool* show);
	~InputMappingsPanel() = default;

	void OnImGuiRender() override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;

private:
	void ReadMappingsFile();
	void SaveMappingsFile();

	void DrawAction(InputActionDefinition& action, int actionIndex);
	void DrawBinding(InputBinding& binding, InputActionValueType valueType, int actionIndex, int bindingIndex);
	bool PollForCapture(InputBinding& binding);

	bool OnOpenDocumentChanged(Event& event) { ReadMappingsFile(); return false; }

private:
	bool* m_Show;
	bool m_WasShown = false;
	bool m_Dirty = false;

	std::vector<InputActionDefinition> m_Actions;

	int m_ListeningActionIndex = -1;
	int m_ListeningBindingIndex = -1;
	bool m_ReadyToCapture = false;

	std::pair<int, int> m_PendingRemoveBinding = { -1, -1 };
	int m_PendingRemoveAction = -1;
};
