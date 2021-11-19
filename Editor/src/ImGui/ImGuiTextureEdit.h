#pragma once

#include "imgui/imgui.h"

#include "Renderer/Texture.h"

namespace ImGui
{
	IMGUI_API bool Texture2DEdit(const char* label, Ref<Texture2D>& texture);
}
