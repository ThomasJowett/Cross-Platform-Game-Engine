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

	static void SetFocussedWindow(Layer* focussedPanel) { s_CurrentlyFoccusedPanel = focussedPanel; }

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

	static Layer* s_CurrentlyFoccusedPanel;
};