#pragma once

#include <filesystem>

#include "Viewers/ViewerManager.h"
#include "FileSystem/FileDialog.h"

namespace ImGui
{
	bool FileEdit(const char* label, std::filesystem::path& filepath, const std::vector<DialogFilterItem>& filters);

	bool FileEdit(const char* label, std::filesystem::path& filepath, ::FileType filetype);

	bool FileSelect(const char* label, std::filesystem::path& filepath, ::FileType filetype);
}