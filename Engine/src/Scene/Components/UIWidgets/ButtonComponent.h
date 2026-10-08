#pragma once

#include "cereal/cereal.hpp"

#include "Asset/Texture.h"

#include "WidgetComponent.h"

#include "Utilities/SerializationUtils.h"
#include "Scripting/Lua/LuaBindings.h"

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

	Texture::FilterMethod normalFilterMethod = Texture::FilterMethod::Nearest;
	Texture::FilterMethod hoveredFilterMethod = Texture::FilterMethod::Nearest;
	Texture::FilterMethod clickedFilterMethod = Texture::FilterMethod::Nearest;
	Texture::FilterMethod disabledFilterMethod = Texture::FilterMethod::Nearest;
	Texture::WrapMethod normalWrapMethod = Texture::WrapMethod::Repeat;
	Texture::WrapMethod hoveredWrapMethod = Texture::WrapMethod::Repeat;
	Texture::WrapMethod clickedWrapMethod = Texture::WrapMethod::Repeat;
	Texture::WrapMethod disabledWrapMethod = Texture::WrapMethod::Repeat;

	REFLECT_LUA_BEGIN(ButtonComponent)
		REFLECT_LUA_PROPERTY(normalTint, "Tint applied to the normal state's texture")
		REFLECT_LUA_PROPERTY(hoveredTint, "Tint applied to the hovered state's texture")
		REFLECT_LUA_PROPERTY(clickedTint, "Tint applied to the clicked state's texture")
		REFLECT_LUA_PROPERTY(disabledTint, "Tint applied to the disabled state's texture")
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		SerializationUtils::SaveTextureToArchive(archive, normalTexture, normalFilterMethod, normalWrapMethod);
		SerializationUtils::SaveTextureToArchive(archive, hoveredTexture, hoveredFilterMethod, hoveredWrapMethod);
		SerializationUtils::SaveTextureToArchive(archive, clickedTexture, clickedFilterMethod, clickedWrapMethod);
		SerializationUtils::SaveTextureToArchive(archive, disabledTexture, disabledFilterMethod, disabledWrapMethod);
		archive(normalTint, hoveredTint, clickedTint, disabledTint);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadTextureFromArchive(archive, normalTexture, normalFilterMethod, normalWrapMethod);
		SerializationUtils::LoadTextureFromArchive(archive, hoveredTexture, hoveredFilterMethod, hoveredWrapMethod);
		SerializationUtils::LoadTextureFromArchive(archive, clickedTexture, clickedFilterMethod, clickedWrapMethod);
		SerializationUtils::LoadTextureFromArchive(archive, disabledTexture, disabledFilterMethod, disabledWrapMethod);
		archive(normalTint, hoveredTint, clickedTint, disabledTint);
	}
};
