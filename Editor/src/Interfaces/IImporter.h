#pragma once

#include <filesystem>

class IImporter
{
public:
	virtual void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination) = 0;
};
