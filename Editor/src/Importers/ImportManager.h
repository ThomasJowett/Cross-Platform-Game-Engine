#pragma once

#include <filesystem>

class ImportManager
{
public:
	static void ImportAsset(const std::filesystem::path& asset, const std::filesystem::path& destination);
	static void ImportMultiAssets(const std::vector<std::filesystem::path>& assets, const std::filesystem::path& destination);
	static void ImportMultiAssets(const std::vector<std::wstring>& assets, const std::filesystem::path& destination);
};