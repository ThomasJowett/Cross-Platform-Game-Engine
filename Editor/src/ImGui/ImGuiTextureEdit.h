#pragma once

#include "imgui/imgui.h"

#include "Renderer/Texture.h"

namespace ImGui
{
	bool Texture2DEdit(const char* label, Ref<Texture2D>& texture, const ImVec2& size = ImVec2(64.0f, 64.0f));
}
