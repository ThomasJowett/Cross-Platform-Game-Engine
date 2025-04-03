#pragma once

#include "cereal/cereal.hpp"

#include "Asset/Texture.h"

#include "WidgetComponent.h"

#include "Utilities/SerializationUtils.h"

struct ButtonComponent
{
	Ref<Texture2D> icon;
	Ref<Texture2D> normalTexture;
	Ref<Texture2D> hoveredTexture;
	Ref<Texture2D> clickedTexture;
	Ref<Texture2D> disabledTexture;

	Colour normalTint = Colours::WHITE;
	Colour hoveredTint = Colours::WHITE;
	Colour clickedTint = Colours::WHITE;
	Colour disabledTint = Colours::WHITE;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		SerializationUtils::SaveTextureToArchive(archive, normalTexture);
		SerializationUtils::SaveTextureToArchive(archive, hoveredTexture);
		SerializationUtils::SaveTextureToArchive(archive, clickedTexture);
		SerializationUtils::SaveTextureToArchive(archive, disabledTexture);
		archive(normalTint, hoveredTint, clickedTint, disabledTint);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadTextureFromArchive(archive, normalTexture);
		SerializationUtils::LoadTextureFromArchive(archive, hoveredTexture);
		SerializationUtils::LoadTextureFromArchive(archive, clickedTexture);
		SerializationUtils::LoadTextureFromArchive(archive, disabledTexture);
		archive(normalTint, hoveredTint, clickedTint, disabledTint);
	}
};
