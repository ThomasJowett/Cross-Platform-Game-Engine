#pragma once

#include "include.h"

class ImportManager
{
public:
	static void ImportAsset(const std::filesystem::path& asset);
	static void ImportMultiAssets(const std::vector<std::string>& assests);
};