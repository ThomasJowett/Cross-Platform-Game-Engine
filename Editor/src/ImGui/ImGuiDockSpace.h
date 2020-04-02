#pragma once

#include "Core/Layer.h"

class ImGuiDockSpace :
	public Layer
{
public:
	ImGuiDockSpace();
	~ImGuiDockSpace() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;

	virtual void OnImGuiRender() override;

	bool* GetShowConsole() { return &m_ShowConsole; }
	bool* GetShowErrorList() { return &m_ShowErrorList; }
	bool* GetShowTaskList() { return &m_ShowTaskList; }
	bool* GetShowProperties() { return &m_ShowProperties; }
	bool* GetShowHierachy() { return &m_ShowHierachy; }
	bool* GetShowContentExplorer() { return &m_ShowContentExplorer; }

private:
	bool m_Show;
	bool m_ShowConsole;
	bool m_ShowErrorList;
	bool m_ShowTaskList;
	bool m_ShowProperties;
	bool m_ShowHierachy;
	bool m_ShowContentExplorer;
};