#pragma once
#include <filesystem>
namespace FileUtils
{
	std::filesystem::path RelativePath(const std::filesystem::path& path, const std::filesystem::path& relative_to);

	// True for absolute paths from any platform, including "C:/..." on Linux and macOS
	bool IsAbsolutePath(const std::filesystem::path& path);

	// Makes an absolute path relative to the project if it is in the project, or if its tail
	// (e.g. "Scripts/Player.lua" from "F:/OldMachine/Project/Scripts/Player.lua") exists in the project
	std::filesystem::path ToProjectRelative(const std::filesystem::path& path, const std::filesystem::path& projectDirectory);
}
