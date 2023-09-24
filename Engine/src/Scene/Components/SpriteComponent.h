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
		SerializationUtils::SaveTextureToArchive(archive, texture);
		archive(tint);
		archive(tilingFactor);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadTextureFromArchive(archive, texture);
		archive(tint);
		archive(tilingFactor);
	}
};
