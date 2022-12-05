#pragma once

#include "Core/core.h"
#include "Core/Application.h"
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
		asset = AssetManager::Get().m_Assets.Load<T>(filepath);
		return std::dynamic_pointer_cast<T>(asset);
	}

	static Ref<Texture2D> GetTexture(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		return AssetManager::Get().m_Textures.Load(filepath);
	}

	static void CleanUp()
	{
		AssetManager::Get().m_Assets.CleanUnused();
	}

private:
	AssetManager():m_Assets(Application::GetOpenDocumentDirectory()) {};
	~AssetManager() = default;
	static AssetManager& Get();

	AssetLibrary m_Assets;
	TextureLibrary2D m_Textures;

	static AssetManager* s_Instance;
};
