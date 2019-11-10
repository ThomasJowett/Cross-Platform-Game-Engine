#pragma once
#include "Core/Layer.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

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
	bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	bool OnMouseMotionEvent(MouseMotionEvent& e);
	bool OnMouseWheelEvent(MouseWheelEvent& e);

	bool OnKeyPressedEvent(KeyPressedEvent& e);
	bool OnKeyReleasedEvent(KeyReleasedEvent& e);
	bool OnKeyTypedEvent(KeyTypedEvent& e);

	bool OnWindowResize(WindowResizeEvent& e);
private:
	float m_time = 0.0f;
};

