#pragma once

#include <filesystem>

class TiledImporter
{
public:
	static void ImportTilemap(const std::filesystem::path& filepath, const std::filesystem::path& destination);
	static void ImportTileset(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};
