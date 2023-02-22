#pragma once

#include <filesystem>

class glTFImporter
{
public:
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};
