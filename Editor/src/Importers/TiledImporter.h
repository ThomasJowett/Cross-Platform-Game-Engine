#pragma once

#include <filesystem>

class TiledImporter
{
public:
	// Inherited via IImporter
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};
