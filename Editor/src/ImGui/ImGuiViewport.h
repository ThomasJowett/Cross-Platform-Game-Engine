#pragma once

#include "include.h"
#include "imgui/imgui.h"

class ImGuiViewportPanel
	:public Layer
{
public: 
	explicit ImGuiViewportPanel(bool* show, Ref<FrameBuffer> framebuffer);
	~ImGuiViewportPanel() = default;

	void OnFixedUpdate() override;
	void OnImGuiRender() override;
private:
	bool* m_Show;
	ImVec2 m_ViewportSize;
	Ref<FrameBuffer> m_FrameBuffer;
	Vector2f m_RelativeMousePosition;
};