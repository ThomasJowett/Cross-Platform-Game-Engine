#pragma once
#include "Core/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

#include "ImGuiConsole.h"

class ImGuiLayer :
	public Layer
{
public:
	ImGuiLayer();
	virtual ~ImGuiLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;
	
	virtual void OnImGuiRender() override;

	void Begin();
	void End();

private:
	bool m_UsingImGui;
};

