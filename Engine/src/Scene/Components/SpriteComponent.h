#pragma once

#include "Core/Colour.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Scene/AssetManager.h"

struct SpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<Texture2D> texture;
	float tilingFactor = 1.0f;

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath;
		if (texture && !texture->GetFilepath().empty())
		{
			relativePath = FileUtils::RelativePath(texture->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		}
		archive(relativePath);
		if (!relativePath.empty())
		{
			archive((int)texture->GetFilterMethod());
			archive((int)texture->GetWrapMethod());
		}
		archive(tint);
		archive(tilingFactor);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
		{
			texture = AssetManager::GetTexture(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
			int filterMethod, wrapMethod;
			archive(filterMethod);
			archive(wrapMethod);
			if (texture)
			{
				texture->SetFilterMethod((Texture::FilterMethod)filterMethod);
				texture->SetWrapMethod((Texture::WrapMethod)wrapMethod);
			}
		}
		else
		{
			texture.reset();
		}
		archive(tint);
		archive(tilingFactor);
	}
};
