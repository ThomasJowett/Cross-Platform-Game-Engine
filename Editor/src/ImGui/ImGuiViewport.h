#pragma once

#include "include.h"
#include "imgui/imgui.h"

class ImGuiViewportPanel
	:public Layer
{
public: 
	explicit ImGuiViewportPanel(bool* show, Ref<FrameBuffer> framebuffer);
	~ImGuiViewportPanel() = default;

	void OnImGuiRender() override;
private:
	bool* m_Show;
	ImVec2 m_WindowSize;
	Ref<FrameBuffer> m_FrameBuffer;
};