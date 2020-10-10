#include "ImportManager.h"

#include "FbxImporter.h"
#include "OBJImporter.h"

void ImportManager::ImportAsset(const std::filesystem::path& asset, const std::filesystem::path& destination)
{
	std::string extString = asset.extension().string();

	std::transform(extString.begin(), extString.end(), extString.begin(), ::tolower);

	const char* ext = extString.c_str();

	if (extString == ".fbx")
	{
		FbxImporter::ImportAssets(asset, destination);
	}
	else if (extString == ".obj")
	{
		OBJImporter::ImportAssets(asset, destination);
	}
}

void ImportManager::ImportMultiAssets(const std::vector<std::filesystem::path>& assets, const std::filesystem::path& destination)
{
	for (auto asset : assets)
	{
		ImportAsset(asset, destination);
	}
}

void ImportManager::ImportMultiAssets(const std::vector<std::wstring>& assets, const std::filesystem::path& destination)
{
	for (auto asset : assets)
	{
		ImportAsset(asset, destination);
	}
}
