#pragma once

#include "include.h"
#include "imgui/imgui.h"

class Viewport
{
public: 
	explicit Viewport();
	~Viewport();

	void Render(const char* title, const ImVec2& size = ImVec2());
private:
	ImVec2 m_WindowSize;
	Ref<FrameBuffer> m_FrameBuffer;
};