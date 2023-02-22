#include "ImportManager.h"

#include "FbxImporter.h"
#include "glTFImporter.h"
#include "OBJImporter.h"
#include "TiledImporter.h"

#include <algorithm>

void ImportManager::ImportAsset(const std::filesystem::path& asset, const std::filesystem::path& destination)
{
	std::string extString = asset.extension().string();

	std::transform(extString.begin(), extString.end(), extString.begin(), ::tolower);

	if (extString == ".fbx")
	{
		FbxImporter::ImportAssets(asset, destination);
	}
	else if (extString == ".obj")
	{
		OBJImporter::ImportAssets(asset, destination);
	}
	else if (extString == ".tmx")
	{
		TiledImporter::ImportTilemap(asset, destination);
	}
	else if (extString == ".tsx")
	{
		TiledImporter::ImportTileset(asset, destination);
	}
	else if (extString == ".gltf" || extString == ".glb")
	{
		glTFImporter::ImportAssets(asset, destination);
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