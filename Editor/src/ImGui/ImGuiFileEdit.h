#pragma once

#include <filesystem>
#include "imgui/imgui.h"

namespace ImGui
{
	IMGUI_API bool FileEdit(const char* label, std::filesystem::path& filepath, const wchar_t* filter);
}