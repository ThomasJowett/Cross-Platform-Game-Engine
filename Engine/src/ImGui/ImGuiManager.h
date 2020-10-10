#pragma once
#include "Core/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

#include "ImGuiConsole.h"

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

private:
	bool m_UsingImGui;

	std::string m_IniFile;
};