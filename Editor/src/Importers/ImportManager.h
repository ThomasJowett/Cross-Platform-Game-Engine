#pragma once

#include "include.h"

class ImportManager
{
public:
	static void ImportAsset(const std::filesystem::path& asset, const std::filesystem::path& destination);
	static void ImportMultiAssets(const std::vector<std::string>& assets, const std::filesystem::path& destination);
};