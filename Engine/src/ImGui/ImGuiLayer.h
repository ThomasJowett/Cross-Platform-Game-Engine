#pragma once
#include "Core/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

class ImGuiLayer :
	public Layer
{
public:
	ImGuiLayer();
	virtual ~ImGuiLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	
	virtual void OnImGuiRender() override;

	void Begin();
	void End();
};

