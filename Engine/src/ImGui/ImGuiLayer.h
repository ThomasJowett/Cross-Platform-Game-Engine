#pragma once
#include "Core/Layer.h"

class DLLIMPEXP_CORE ImGuiLayer :
	public Layer
{
public:
	ImGuiLayer();
	virtual ~ImGuiLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;
	virtual void OnEvent(Event& event) override;

private:
	float m_time = 0.0f;
};

