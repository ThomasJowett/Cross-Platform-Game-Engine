#pragma once

#include <filesystem>

namespace ExternalEditor
{
#ifdef __EMSCRIPTEN__
	constexpr bool IsSupported() { return false; }
#else
	constexpr bool IsSupported() { return true; }
#endif

	// Opens the file in the OS default application for its file type
	bool OpenFile(const std::filesystem::path& filepath);

	// Draws an "Open Externally" menu item for the file, if supported on this platform
	void MenuItem(const std::filesystem::path& filepath);
}
