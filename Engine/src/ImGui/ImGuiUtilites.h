#pragma once

#include <filesystem>

#include "imgui/imgui.h"
#include "Renderer/Texture.h"

namespace ImGui
{
	IMGUI_API void Image(Ref<Texture> texture, const ImVec2& size, const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	IMGUI_API bool ImageButton(Ref<Texture> texture, const ImVec2& size, int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	bool SaveStyle(const std::filesystem::path& filename, const ImGuiStyle& style);
	bool LoadStyle(const std::filesystem::path& filename, ImGuiStyle& style);

	void Tooltip(const std::string& text);
	void Tooltip(const char* text);
}