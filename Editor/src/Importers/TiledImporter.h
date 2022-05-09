#pragma once
#include "Interfaces/IImporter.h"

class TiledImporter : public IImporter
{
public:
	// Inherited via IImporter
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};
