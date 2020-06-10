#pragma once

#include "imgui/imgui.h"
#include "Renderer/Texture.h"

namespace ImGui
{
	IMGUI_API void Image(Ref<Texture> texture, const ImVec2& size, const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	IMGUI_API bool ImageButton(Ref<Texture> texture, const ImVec2& size, int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool SaveStyle(const char* filename, const ImGuiStyle& style);
	bool LoadStyle(const char* filename, ImGuiStyle& style);
}