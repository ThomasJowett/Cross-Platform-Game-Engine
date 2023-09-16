#pragma once

#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

class ImGuiManager
{
public:
	ImGuiManager();
	virtual ~ImGuiManager() = default;

	virtual void Init();
	virtual void Shutdown();
	virtual void OnEvent(Event& event);

	void Begin();
	void End();

	bool IsUsing() { return m_UsingImGui; }
	void SetIsUsing(bool isUsing) { m_UsingImGui = isUsing; }

	static void SetOverrideMouseCursor(bool overrideCursor);

private:
	bool m_UsingImGui;

	std::string m_IniFile;
};