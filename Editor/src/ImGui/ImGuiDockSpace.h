#pragma once

#include "Core/Layer.h"
#include "include.h"

class ImGuiDockSpace :
	public Layer
{
public:
	ImGuiDockSpace();
	~ImGuiDockSpace() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;
	virtual void OnUpdate(float deltaTime) override;

	virtual void OnImGuiRender() override;

	bool* GetShowEditorPreferences() { return &m_ShowEditorPreferences; }
	bool* GetShowViewport() { return &m_ShowViewport; }
	bool* GetShowConsole() { return &m_ShowConsole; }
	bool* GetShowErrorList() { return &m_ShowErrorList; }
	bool* GetShowTaskList() { return &m_ShowTaskList; }
	bool* GetShowProperties() { return &m_ShowProperties; }
	bool* GetShowHierachy() { return &m_ShowHierachy; }
	bool* GetShowContentExplorer() { return &m_ShowContentExplorer; }
	bool* GetShowJoystickInfo() { return &m_ShowJoystickInfo; }

private:
	bool m_Show;

	bool m_ShowEditorPreferences;

	bool m_ShowViewport;
	bool m_ShowConsole;
	bool m_ShowErrorList;
	bool m_ShowTaskList;
	bool m_ShowProperties;
	bool m_ShowHierachy;
	bool m_ShowContentExplorer;
	bool m_ShowJoystickInfo;
};