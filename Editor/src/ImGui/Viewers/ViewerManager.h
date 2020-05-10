#include "include.h"

class ViewerManager
{
public:
	static Ref<Layer> GetAssetViewer(std::filesystem::path assetPath);

	static void OpenViewer(Ref<Layer> viewer);
	static void CloseViewer(Ref<Layer> viewer);

private:

	static std::unordered_map<std::filesystem::path, Ref<Layer>> s_AssetViewers;
};
