#pragma once
#include <filesystem>
namespace FileUtils
{
	std::filesystem::path RelativePath(const std::filesystem::path& path, const std::filesystem::path& relative_to);
}