#pragma once

#include "Interfaces/IImporter.h"

class FbxImporter : public IImporter
{
public:
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};