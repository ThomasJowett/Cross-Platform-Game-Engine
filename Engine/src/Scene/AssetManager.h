#pragma once

#include "Core/core.h"

#include "Core/Asset.h"
#include "Logging/Instrumentor.h"
#include "Renderer/Texture.h"
#include "Core/Factory.h"

class AssetManager
{
public:
	template<typename T>
	static Ref<T> GetAsset(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		Ref<Asset> asset = nullptr;
		asset = s_Assets.Load<T>(filepath);
		return std::dynamic_pointer_cast<T>(asset);
	}

	static Ref<Texture2D> GetTexture(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		return s_Textures.Load(filepath);
	}

	static void CleanUp()
	{
		s_Assets.CleanUnused();
	}

private:
	AssetManager() {};
	~AssetManager() = default;
	static AssetManager& Get();

	static AssetLibrary s_Assets;
	static TextureLibrary2D s_Textures;

	static AssetManager* s_Instance;
};
