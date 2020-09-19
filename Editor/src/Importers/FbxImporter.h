#pragma once

#include <filesystem>

class FbxImporter
{
public:
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};