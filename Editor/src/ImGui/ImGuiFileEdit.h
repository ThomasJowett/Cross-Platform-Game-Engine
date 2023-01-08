#pragma once

#include <filesystem>

#include "Viewers/ViewerManager.h"

namespace ImGui
{
	bool FileEdit(const char* label, std::filesystem::path& filepath, const wchar_t* filter);

	bool FileEdit(const char* label, std::filesystem::path& filepath, ::FileType filetype);

	bool FileSelect(const char* label, std::filesystem::path& filepath, ::FileType filetype);
}