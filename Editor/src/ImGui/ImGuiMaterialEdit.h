#pragma once

#include "imgui/imgui.h"
#include "Renderer/Material.h"

namespace ImGui
{
	IMGUI_API bool MaterialEdit(const char* label, Material& material);
}

